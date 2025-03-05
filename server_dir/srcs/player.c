#include "../includes/server.h"

void close_invalid_client(int client_socket, const char *message)
{
    log_printf(PRINT_ERROR, "%s\n", message);
    close(client_socket);
}

int validate_team(const char *team_name, server_config_t *config)
{
    for (int i = 0; i < config->team_count; i++)
    {
        if (strcmp(team_name, config->teams[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int count_players_in_team(player_t *players[], const char *team_name)
{
    int i = 0;
    int count = 0;
    while (players[i] != NULL)
    {
        if (strcmp(players[i]->team_name, team_name) == 0)
        {
            count++;
        }
        i++;
    }
    return count;
}

void log_printf_identity(print_type type, player_t *player, const char *format, ...)
{
    log_printf(type, "[\"%d\"][L%d].[%s].[N%d][C%d], ", player->socket, player->level, player->team_name, player->inventory.nourriture, player->life_cycle);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

player_t init_player(int client_socket, egg_t *eggs[], int *egg_id, int *egg_count, const char *team_name, server_config_t *config)
{
    player_t player;
    bool egg_found = false;

    for (int i = 0; i < *egg_count; i++)
    {
        if (eggs[i] != NULL)
        {
            if (strcmp(eggs[i]->team_name, team_name) == 0 && eggs[i]->time_before_hatch == 0)
            {
                egg_found = true;
                *egg_id = eggs[i]->id;
                player.x = eggs[i]->x;
                player.y = eggs[i]->y;
                remove_egg(eggs, egg_count, team_name, player.x, player.y);
                break;
            }
        }
    }

    if (!egg_found)
    {
        player.x = rand() % config->width;
        player.y = rand() % config->height;
    }

    player.socket = client_socket;
    strncpy(player.team_name, team_name, sizeof(player.team_name) - 1);
    player.team_name[sizeof(player.team_name) - 1] = '\0';
    player.direction = rand() % 4;
    player.action_count = 0;
    player.current_execution_time = 0;
    player.level = 1;
    player.need_level_up = false;
    player.incantation_trigger = false;
    player.life_cycle = 1;
    player.inventory.nourriture = 10;
    player.inventory.linemate = 0;
    player.inventory.deraumere = 0;
    player.inventory.sibur = 0;
    player.inventory.mendiane = 0;
    player.inventory.phiras = 0;
    player.inventory.thystame = 0;
    return player;
}

void assign_new_player(int graphic_socket, int client_socket, player_t *players[], egg_t *eggs[], int *egg_count, int max_players, const char *team_name, server_config_t *config)
{
    for (int i = 0; i < max_players; i++)
    {
        if (players[i] == NULL)
        {
            players[i] = calloc(1, sizeof(player_t));
            int previous_egg_count = *egg_count;
            int egg_id = -1;
            *players[i] = init_player(client_socket, eggs, &egg_id, egg_count, team_name, config);
            send_message_player(*players[i], "BIENVENUE\n");
            char *dir = get_player_direction(players[i]);
            log_printf_identity(PRINT_INFORMATION, players[i], "est place en position [%d, %d], direction %s\n", players[i]->x, players[i]->y, dir);
            dprintf(client_socket, "%d %d\n", players[i]->x, players[i]->y);
            free(dir);
            send_graphic_new_player(graphic_socket, players[i], previous_egg_count != *egg_count, egg_id);
            return;
        }
    }
    close_invalid_client(client_socket, "Trop de joueurs connectés");
}

void accept_new_client(int server_socket, player_t *players[], egg_t *eggs[], int *egg_count, int max_clients, server_config_t *config, map_t *map, int *graphic_socket, bool *game_started)
{
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

    if (client_socket < 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors de accept\n");
        return;
    }

    log_printf(PRINT_INFORMATION, "Nouvelle connexion acceptée (socket %d)\n", client_socket);

    char buffer[BUFFER_SIZE];
    int total = 0;
    memset(buffer, 0, BUFFER_SIZE);
    while (1)
    {
        char small_buffer[BUFFER_SIZE_SMALL];
        ssize_t bytes_read = read(client_socket, small_buffer, sizeof(small_buffer) - 1);
        // printf("Correction - Small[%s]\n", small_buffer);
        if (bytes_read < 0)
        {
            log_printf(PRINT_INFORMATION, "Client déconnecté (socket %d)\n", client_socket);
            break;
        }
        small_buffer[bytes_read] = '\0';
        size_t space_left = BUFFER_SIZE - 1 - total;
        if ((size_t)bytes_read > space_left)
        {
            bytes_read = space_left;
        }
        memcpy(buffer + total, small_buffer, bytes_read);
        total += bytes_read;
        buffer[total] = '\0';
        if (strchr(buffer, '\n') != NULL)
        {
            break;
        }
        if (total >= BUFFER_SIZE - 1)
        {
            break;
        }
    }
    // printf("CORRECTION - BUFFER FINAL[%s]\n", buffer);

    buffer[strcspn(buffer, "\n")] = 0;
    if (strcmp(buffer, "GRAPHIC") == 0)
    {
        if (*graphic_socket != -1)
        {
            log_printf(PRINT_ERROR, "Un client graphique est déjà connecté\n");
            close(client_socket);
            return;
        }
        *graphic_socket = client_socket;
        *game_started = true;
        log_printf(PRINT_INFORMATION, "Client graphique connecté (socket %d)\n", client_socket);
        send_initial_graphic_data(*graphic_socket, config, map, players, max_clients, eggs, *egg_count);
        return;
    }

    if (!validate_team(buffer, config))
    {
        close_invalid_client(client_socket, "Équipe invalide");
        return;
    }

    if (!(*game_started)) {
        int count = 0;
        for (int i = 0; i < max_clients; i++) {
            if (players[i] != NULL)
                count++;
        }

        if (count + 1> config->clients_per_team) {
            close_invalid_client(client_socket,
                "Équipe pleine");
            return;
        }
    }

    assign_new_player(*graphic_socket, client_socket, players, eggs, egg_count, max_clients, buffer, config);
}

void free_player(player_t *player)
{
    if (player == NULL)
        return;

    close(player->socket);

    for (int i = 0; i < player->action_count; i++)
    {
        free(player->actions[i]);
        player->actions[i] = NULL;
    }

    free(player);
}

void free_players(player_t *players[])
{
    int player_count = 0;

    while (players[player_count] != NULL)
    {
        free_player(players[player_count]);
        player_count++;
    }
}

void add_action_to_player(player_t *player, const char *action)
{
    if (player->action_count >= MAX_ACTIONS)
    {
        log_printf(PRINT_ERROR, "Joueur %d : file d'attente pleine, actione ignorée\n", player->socket);
        return;
    }
    player->actions[player->action_count] = strdup(action);
    player->action_count++;
}

void print_players(player_t *players[], int max_players)
{
    printf("- - - - - Liste des joueurs - - - - -\n");
    for (int i = 0; i < max_players; i++)
    {
        if (players[i] != NULL)
        {
            printf("Index: %d | Socket: %d | Team: %s | Position: [%d, %d] | Level : [%d]\n",
                   i, players[i]->socket, players[i]->team_name, players[i]->x, players[i]->y, players[i]->level);
            if (players[i]->action_count > 0)
            {
                printf("Action: ");
                for (int j = 0; j < players[i]->action_count; j++)
                {
                    if (players[i]->actions[j] != NULL)
                        printf("[%s]", players[i]->actions[j]);
                }
                printf("\n");
            }
        }
        else
        {
            printf("Index: %d | Empty slot\n", i);
        }
    }
    printf("- - - - - - - - - - - - - - - - - - -\n");
}

bool player_eat(int graphic_socket, player_t *player)
{
    player->life_cycle--;
    if (player->life_cycle <= 0)
    {
        if (player->inventory.nourriture > 0)
        {
            send_graphic_player_inventory(graphic_socket, player);
            player->inventory.nourriture--;
            player->life_cycle = 126;
            return true;
        }
        log_printf_identity(PRINT_INFORMATION, player, "est mort de faim\n");
        send_message_player(*player, "mort\n");
        send_graphic_player_death(graphic_socket, player);
        return false;
    }
    return true;
}

bool are_players_dead(player_t *players[], int max_players)
{
    for (int i = 0; i < max_players; i++)
    {
        if (players[i] != NULL && players[i]->life_cycle > 0)
        {
            return false;
        }
    }
    return true;
}
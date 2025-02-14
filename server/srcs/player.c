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
    }
    return count;
}

void log_printf_identity(print_type type, player_t *player, const char *format, ...)
{
    log_printf(type, "[%d][%s], ", player->socket, player->team_name);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

player_t init_player(int client_socket, const char *team_name, server_config_t *config)
{
    player_t player;

    player.socket = client_socket;
    strncpy(player.team_name, team_name, sizeof(player.team_name) - 1);
    player.team_name[sizeof(player.team_name) - 1] = '\0';
    player.x = rand() % config->width;
    player.y = rand() % config->height;
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

void assign_new_player(int client_socket, player_t *players[], int max_players, const char *team_name, server_config_t *config)
{
    for (int i = 0; i < max_players; i++)
    {
        if (players[i] == NULL)
        {
            players[i] = malloc(sizeof(player_t));
            *players[i] = init_player(client_socket, team_name, config);

            send_message_player(*players[i], "BIENVENUE\n");
            log_printf_identity(PRINT_INFORMATION, players[i], "est place en position [%d, %d], direction %s\n", players[i]->x, players[i]->y, get_player_direction(players[i]));
            dprintf(client_socket, "%d %d\n", players[i]->x, players[i]->y);
            return;
        }
    }
    close_invalid_client(client_socket, "Trop de joueurs connectés");
}

void accept_new_client(int server_socket, player_t *players[], int max_clients, server_config_t *config, int *graphic_socket, bool *game_started)
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
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0)
    {
        log_printf(PRINT_ERROR, "Erreur de lecture du client (socket %d)\n", client_socket);
        close(client_socket);
        return;
    }

    buffer[bytes_read] = '\0';
    buffer[strcspn(buffer, "\n")] = 0;

    // Vérifier si c'est le client graphique
    if (strcmp(buffer, "GRAPHIC") == 0)
    {
        if (*graphic_socket != -1)
        {
            log_printf(PRINT_ERROR, "Un client graphique est déjà connecté\n");
            close(client_socket);
            return;
        }
        *graphic_socket = client_socket;
        *game_started = true; // La partie peut commencer
        log_printf(PRINT_INFORMATION, "Client graphique connecté (socket %d)\n", client_socket);
        return;
    }

    // Vérifier si l'équipe est valide
    if (!validate_team(buffer, config))
    {
        close_invalid_client(client_socket, "Équipe invalide");
        return;
    }

    // Vérifier si l'équipe a encore de la place
    if (game_started == false && count_players_in_team(players, buffer) >= config->clients_per_team)
    {
        close_invalid_client(client_socket, "Équipe pleine");
        return;
    }

    assign_new_player(client_socket, players, max_clients, buffer, config);
}

void free_player(player_t *player)
{
    close(player->socket);
    for (int i = 0; i < player->action_count; i++)
    {
        free(player->actions[i]);
    }
    free(player);
    player = NULL;
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
            printf("Index: %d | Socket: %d | Team: %s | Position: [%d, %d]\n",
                   i, players[i]->socket, players[i]->team_name, players[i]->x, players[i]->y);
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

bool player_eat(player_t *player)
{
    player->life_cycle--;
    if (player->life_cycle <= 0)
    {
        if (player->inventory.nourriture > 0)
        {
            player->inventory.nourriture--;
            player->life_cycle = 126;
            return true;
        }
        log_printf_identity(PRINT_INFORMATION, player, "est mort de faim\n");
        send_message_player(*player, "mort\n");
        return false;
    }
    return true;
}
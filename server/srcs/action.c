#include "../includes/server.h"

void execute_player_action(player_t *player, map_t *map, player_t *players[], int max_players, egg_t *eggs, int *egg_count)
{
    if (player->current_execution_time > 1)
    {
        player->current_execution_time--;
        return;
    }
    if (player->incantation_trigger == true)
    {
        player->incantation_trigger = false;
        level_up_players(players, max_players);
    }
    if (player->action_count == 0)
        return;
    player->current_execution_time = action_switch(player, player->actions[0], map, players, max_players, eggs, egg_count);
    free(player->actions[0]);
    for (int i = 1; i < player->action_count; i++)
    {
        player->actions[i - 1] = player->actions[i];
    }
    player->actions[player->action_count - 1] = NULL;
    player->action_count--;
}

int action_switch(player_t *player, char *action, map_t *map, player_t *players[], int max_players, egg_t *eggs, int *egg_count)
{

    log_printf_identity(PRINT_RECEIVE, player, "a [serveur] -> %s\n", action);
    if (strcmp(action, "avance") == 0)
        return action_move_forward(player, map);
    if (strcmp(action, "droite") == 0)
        return action_turn(player, false);
    if (strcmp(action, "gauche") == 0)
        return action_turn(player, true);
    if (strcmp(action, "voir") == 0)
        return action_see(player, map, players, max_players);
    if (strcmp(action, "inventaire") == 0)
        return action_inventory(player);
    if (strncmp(action, "prend", 5) == 0)
        return action_take(player, map, action);
    if (strncmp(action, "pose", 4) == 0)
        return action_put(player, map, action);
    if (strcmp(action, "expulse") == 0)
        return action_kick(player, map, players, max_players);
    if (strncmp(action, "broadcast", 9) == 0)
        return action_broadcast(player, map, players, max_players, action);
    if (strcmp(action, "expulse") == 0)
        return action_incantation(player, map, players, max_players);
    if (strcmp(action, "fork") == 0)
        return action_lay_egg(player, eggs, egg_count);

    log_printf_identity(PRINT_ERROR, player, "a envoye une commande inconnue: %s\n", action);
    send_message_player(*player, "Unknown command\n");
    return 0;
}

int action_move_forward(player_t *player, map_t *map)
{
    move_forward(player, map);
    log_printf_identity(PRINT_INFORMATION, player, "a avancé en direction de %s, en [%d, %d]\n", get_player_direction(player), player->x, player->y);
    send_message_player(*player, "ok\n");
    return 7;
}

int action_turn(player_t *player, bool left)
{
    turn_player(player, left);
    log_printf_identity(PRINT_INFORMATION, player, "tourne à %s, nouvelle direction: %s\n", left ? "gauche" : "droite", get_player_direction(player));
    send_message_player(*player, "ok\n");
    return 7;
}

int action_see(player_t *player, map_t *map, player_t *players[], int max_players)
{
    int visible_cell_count = get_visible_cell_count(player->level);
    int coordinates[visible_cell_count][2];
    get_visible_cells_coordinates(player, map, coordinates);
    char buffer[96 * visible_cell_count];
    get_elements_from_coordinates(map, coordinates, visible_cell_count, buffer, players, max_players);
    int buffer_len = strlen(buffer);
    char package[buffer_len];
    snprintf(package, buffer_len + 4, "{%s}\n", buffer);
    log_printf_identity(PRINT_INFORMATION, player, "a demandé ce qu'il voiyait depuis [%d, %d] en direction du %s\n", player->x, player->y, get_player_direction(player));
    send_message_player(*player, package);
    return 7;
}

int action_inventory(player_t *player)
{
    char buffer[BUFFER_SIZE_SMALL];
    get_player_inventory(player, buffer, sizeof(buffer));
    log_printf_identity(PRINT_INFORMATION, player, "a demandé son inventaire\n");
    send_message_player(*player, buffer);
    return 1;
}

int action_take(player_t *player, map_t *map, const char *action)
{
    int resource_index;
    char *object = strchr(action, ' ') + 1;

    if (object == NULL || (resource_index = get_resource_index(object)) == -1)
    {
        log_printf_identity(PRINT_ERROR, player, "a envoyé une commande invalide: %s\n", action);
        send_message_player(*player, "ko\n");
        return 0;
    }

    int count = ((int *)&map->cells[player->y][player->x].resources)[resource_index];
    if (count <= 0)
    {
        log_printf_identity(PRINT_ERROR, player, "a demandé à prendre l'objet \"%s\", il n'y en a pas en [%d, %d]\n", object, player->x, player->y);
        send_message_player(*player, "ko\n");
        return 0;
    }
    ((int *)&map->cells[player->y][player->x].resources)[resource_index]--;
    ((int *)&player->inventory)[resource_index]++;
    log_printf_identity(PRINT_INFORMATION, player, "a prit l'objet \"%s\", il y en reste %d en [%d, %d], la ou il se trouve (il en a possed %d dans son inventaire)\n", object, ((int *)&map->cells[player->y][player->x].resources)[resource_index], player->x, player->y, ((int *)&player->inventory)[resource_index]);
    return 7;
}

int action_put(player_t *player, map_t *map, const char *action)
{
    int resource_index;
    char *object = strchr(action, ' ') + 1;

    if (object == NULL || (resource_index = get_resource_index(object)) == -1)
    {
        log_printf_identity(PRINT_ERROR, player, "a envoyé une commande invalide: %s\n", action);
        send_message_player(*player, "ko\n");
        return 0;
    }

    int count = ((int *)&player->inventory)[resource_index];
    if (count <= 0)
    {
        log_printf_identity(PRINT_ERROR, player, "a demandé à deposer l'objet \"%s\" en [%d,%d], il n'en a pas\n", object, player->x, player->y);
        send_message_player(*player, "ko\n");
        return 0;
    }
    ((int *)&player->inventory)[resource_index]--;
    ((int *)&map->cells[player->y][player->x].resources)[resource_index]++;
    log_printf_identity(PRINT_INFORMATION, player, "a deposer l'objet \"%s\", il y en a miantenant %d en [%d, %d], la ou il se trouve (il en possede %d dans son inventaire)\n", object, ((int *)&map->cells[player->y][player->x].resources)[resource_index], player->x, player->y, ((int *)&player->inventory)[resource_index]);
    return 7;
}

int action_kick(player_t *player, map_t *map, player_t *players[], int max_players)
{
    bool did_kicked = kick_players(player, map, players, max_players);
    send_message_player(*player, did_kicked ? "ok\n" : "ko\n");
    return 7;
}

int action_broadcast(player_t *player, map_t *map, player_t *players[], int max_players, const char *action)
{
    int player_count = 0;
    char *message = strchr(action, ' ') + 1;
    int cell_source = -1;
    int message_len = strlen(message);

    if (message == NULL)
    {
        log_printf_identity(PRINT_ERROR, player, "a envoyé une commande invalide: %s\n", action);
        send_message_player(*player, "ko\n");
        return 0;
    }
    while (players[player_count] != NULL && player_count < max_players)
    {
        if (players[player_count]->socket != player->socket)
        {
            if ((cell_source = get_sound_direction(player, players[player_count], map)) == -1)
            {
                log_printf_identity(PRINT_ERROR, players[player_count], "ne sait pas d'ou vient le message: %s\n", action);
            }
            else
            {
                char buffer[message_len + 24];
                sprintf(buffer, "message %d, %s\n", cell_source, message);
                send_message_player(*players[player_count], buffer);
            }
        }
        player_count++;
    }
    return 7;
}

int action_incantation(player_t *player, map_t *map, player_t *players[], int max_players)
{
    if (can_incantation(player, map, players, max_players))
    {
        start_incantation(player, map, players, max_players);
        char buffer[BUFFER_SIZE_TINY];
        sprintf(buffer, "elevation en cours niveau actuel : %d\n", player->level);
        send_message_player(*player, buffer);
        return 300;
    }
    send_message_player(*player, "ko\n");
    return 7;
}

int action_lay_egg(player_t *player, egg_t *eggs, int *egg_count)
{
    add_egg(&eggs, egg_count, (egg_t){player->team_name, player->socket, player->x, player->y, 42, 600});
    log_printf_identity(PRINT_INFORMATION, player, "commence a pondre un oeuf en [%d, %d]\n", player->x, player->y);
    send_message_player(*player, "ok\n");
    return 42;
}
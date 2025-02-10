#include "../includes/server.h"

void execute_player_action(player_t *player, map_t *map)
{
    if (player->current_execution_time > 1)
    {
        player->current_execution_time--;
        return;
    }
    if (player->action_count == 0)
        return;
    player->current_execution_time = action_switch(player, player->actions[0], map);
    free(player->actions[0]);
    for (int i = 1; i < player->action_count; i++)
    {
        player->actions[i - 1] = player->actions[i];
    }
    player->actions[player->action_count - 1] = NULL;
    player->action_count--;
}

int action_switch(player_t *player, char *action, map_t *map)
{

    log_printf_identity(PRINT_RECEIVE, player, "a envoye: %s\n", action);
    if (strcmp(action, "avance") == 0)
        return action_move_forward(player, map);
    if (strcmp(action, "droite") == 0)
        return action_turn(player, false);
    if (strcmp(action, "gauche") == 0)
        return action_turn(player, true);
    if (strcmp(action, "voir") == 0)
        return action_see(player, map);
    if (strcmp(action, "inventaire") == 0)
        return action_inventory(player);
    if (strncmp(action, "prend", 5) == 0)
        return action_take(player, map, action);
    if (strncmp(action, "pose", 4) == 0)
        return action_put(player, map, action);
    log_printf_identity(PRINT_ERROR, player, "a envoye une commande inconnue: %s\n", action);
    send_message(player->socket, "Unknown command\n");
    return 0;
}

int action_move_forward(player_t *player, map_t *map)
{
    move_forward(player, map);
    log_printf_identity(PRINT_INFORMATION, player, "a avancé en direction de %s, en [%d, %d]\n", get_player_direction(player), player->x, player->y);
    send_message(player->socket, "ok\n");
    return 7;
}

int action_turn(player_t *player, bool left)
{
    turn_player(player, left);
    log_printf_identity(PRINT_INFORMATION, player, "tourne à %s, nouvelle direction: %s\n", left ? "gauche" : "droite", get_player_direction(player));
    send_message(player->socket, "ok\n");
    return 7;
}

int action_see(player_t *player, map_t *map)
{
    // TMP
    log_printf_identity(PRINT_INFORMATION, player, "a demandé ce qu'il voiyait\n");
    return 7;
}

int action_inventory(player_t *player)
{
    char buffer[BUFFER_SIZE_SMALL];
    get_player_inventory(player, buffer, sizeof(buffer));
    log_printf_identity(PRINT_INFORMATION, player, "a demandé son inventaire\n");
    send_message(player->socket, buffer);
    return 1;
}

int action_take(player_t *player, map_t *map, const char *action)
{
    int resource_index;
    char *object = strchr(action, ' ') + 1;

    if (object == NULL || (resource_index = get_resource_index(object)) == -1)
    {
        log_printf_identity(PRINT_ERROR, player, "a envoyé une commande invalide: %s\n", action);
        send_message(player->socket, "ko\n");
        return 0;
    }
    
    int count = ((int *)&map->cells[player->y][player->x].resources)[resource_index];
    if (count <= 0)
    {
        log_printf_identity(PRINT_ERROR, player, "a demandé à prendre l'objet \"%s\", il n'y en a pas en [%d, %d]\n", object, player->x, player->y);
        send_message(player->socket, "ko\n");
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
        send_message(player->socket, "ko\n");
        return 0;
    }
    
    int count = ((int *)&player->inventory)[resource_index];
    if (count <= 0)
    {
        log_printf_identity(PRINT_ERROR, player, "a demandé à deposer l'objet \"%s\" en [%d,%d], il n'en a pas\n", object, player->x, player->y);
        send_message(player->socket, "ko\n");
        return 0;
    }
    ((int *)&player->inventory)[resource_index]--;
    ((int *)&map->cells[player->y][player->x].resources)[resource_index]++;
    log_printf_identity(PRINT_INFORMATION, player, "a deposer l'objet \"%s\", il y en a miantenant %d en [%d, %d], la ou il se trouve (il en possede %d dans son inventaire)\n", object, ((int *)&map->cells[player->y][player->x].resources)[resource_index], player->x, player->y, ((int *)&player->inventory)[resource_index]);
    return 7;
}

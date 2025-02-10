#include "../includes/server.h"

void execute_player_action(player_t *player, map_t *map)
{
    if (player->current_execution_time > 1) {
        player->current_execution_time--;
        return;
    }

    if (player->action_count == 0)
        return;
    player->current_execution_time = action_switch(player, player->actions[0], map);
    free(player->actions[0]);
    for (int i = 1; i < player->action_count; i++) {
        player->actions[i - 1] = player->actions[i];
    }
    player->actions[player->action_count - 1] = NULL;
    player->action_count--;
}

int action_switch(player_t *player, char *action, map_t *map)
{

    log_printf_identity(PRINT_RECEIVE, player,"a envoye: %s\n", action);
    if (strcmp(action, "avance") == 0) {
        move_forward(player, map);
        log_printf_identity(PRINT_INFORMATION, player, "a avancé en direction de %s, en [%d, %d]\n", get_player_direction(player), player->x, player->y);
        send_message(player->socket, "ok\n");
        return 7;
    } else if (strcmp(action, "droite") == 0) {
        turn_player(player, true);
        log_printf_identity(PRINT_INFORMATION, player, "tourne à %s, nouvelle direction: %s\n",  action, get_player_direction(player));
        send_message(player->socket, "ok\n");
        return 7;
    } else if (strcmp(action, "gauche") == 0) {
        turn_player(player, false);
        log_printf_identity(PRINT_INFORMATION, player, "tourne à %s, nouvelle direction: %s\n",  action, get_player_direction(player));
        send_message(player->socket, "ok\n");
        return 7;

    } else if (strcmp(action, "voir") == 0) {
        //TMP
        log_printf_identity(PRINT_INFORMATION, player, "a demandé ce qu'il voiyait\n");
        return 7;
    } else if (strcmp(action, "inventaire") == 0) {
        char buffer[BUFFER_SIZE_SMALL];
        get_player_inventory(player, buffer, sizeof(buffer));
        log_printf_identity(PRINT_INFORMATION, player, "a demandé son inventaire\n");
        send_message(player->socket, buffer);
        return 1;
    }

    log_printf_identity(PRINT_ERROR, player,  "a envoye une commande inconnue: %s\n",  action);
    send_message(player->socket, "Unknown command\n");
    return 0;
}

void turn_player(player_t *player, bool left) {
    int direction_changer = left ? 3 : 1;
    player->direction = (player->direction + direction_changer) % 4;
}

void move_forward(player_t *player, map_t *map) {
    switch (player->direction) {
        case NORTH:
            player->y = (player->y - 1 + map->height) % map->height;
            break;
        case EAST:
            player->x = (player->x + 1) % map->width;
            break;
        case SOUTH:
            player->y = (player->y + 1) % map->height;
            break;
        case WEST:
            player->x = (player->x - 1 + map->width) % map->width;
            break;
    }
}

void get_player_inventory(player_t *player, char *buffer, size_t size) {
    snprintf(buffer, size, "{nourriture: %d, linemate: %d, deraumere: %d, sibur: %d, mendiane: %d, phiras: %d, thystame: %d}\n", player->inventory.nourriture, player->inventory.linemate, player->inventory.deraumere, player->inventory.sibur,player->inventory.mendiane, player->inventory.phiras, player->inventory.thystame);
}





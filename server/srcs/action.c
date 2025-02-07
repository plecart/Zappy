#include "../includes/server.h"

void execute_player_action(player_t *player) {
    if (player->current_execution_time >= 0) {
        player->current_execution_time--;
        return;
    }

    if (player->action_count == 0)
        return;

//TMP
    log_printf(PRINT_INFORMATION, "TMP -> Exécution de l'action '%s' pour le joueur %d\n", player->actions[0], player->socket);
    send_message(player->socket, "Ok\n");
    player->current_execution_time = 1;
//
    
    free(player->actions[0]);
    for (int i = 1; i < player->action_count; i++) {
        player->actions[i - 1] = player->actions[i];
    }
    player->actions[player->action_count - 1] = NULL;
    player->action_count--;
}

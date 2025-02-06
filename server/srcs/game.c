#include "../includes/server.h"

void assign_player_position(player_t *player, map_t *map) {
    player->x = rand() % map->width;
    player->y = rand() % map->height;
    player->direction = rand() % 4;  // Orientation aléatoire (0 à 3)
}

void *game_loop(void *arg) {
    map_t *map = (map_t *)arg;  // Convertir l'argument en pointeur vers `map_t`
    struct timeval start, end;
    double elapsed;

    while (1) {
        gettimeofday(&start, NULL);

        if (game_time.game_started) {
            game_time.elapsed_time++;
            log_printf("Temps de jeu écoulé : %d unités\n", game_time.elapsed_time);

            for (int i = 0; i < player_count; i++) {
                process_player_command(&players[i], map, 1.0 / game_time.time_unit);
            }
        }

        do {
            gettimeofday(&end, NULL);
            elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        } while (elapsed < (1.0 / game_time.time_unit));
    }
    return NULL;
}


void enqueue_command(player_t *player, const char *command) {
    if (player->command_count < 10) {
        strncpy(player->command_queue[player->command_count], command, BUFFER_SIZE);
        player->command_count++;
    }
}

void process_player_command(player_t *player, map_t *map, double delta_time) {
    if (player->action_time_remaining > 0) {
        player->action_time_remaining -= delta_time;  // Réduire le temps restant
        return;  // Ne pas exécuter de nouvelle commande tant que l’attente n’est pas finie
    }

    if (player->command_count == 0) return;  // Aucune commande en attente

    // Récupérer la commande en attente
    char command[BUFFER_SIZE];
    strncpy(command, player->command_queue[0], BUFFER_SIZE);

    // Décaler la file d’attente
    for (int i = 0; i < player->command_count - 1; i++) {
        strncpy(player->command_queue[i], player->command_queue[i + 1], BUFFER_SIZE);
    }
    player->command_count--;

    // Déterminer le temps d’exécution de la commande
    double execution_time = execute_player_action(player, command, map);

    // Appliquer le délai avant que le joueur puisse exécuter une autre commande
    player->action_time_remaining = execution_time;
}


double execute_player_action( player_t *player, const char *command, map_t *map) {

    if (strcmp(command, "avance\n") == 0) {
        printf_identity(player);
        move_forward(player, map);
        write(player->socket, "ok\n", 3);
        return 1;
    } 
    else if (strcmp(command, "droite\n") == 0) {
        printf_identity(player);
        turn_right(player);
        write(player->socket, "ok\n", 3);
        return 1;
    } 
    else if (strcmp(command, "gauche\n") == 0) {
        printf_identity(player);
        turn_left(player);
        write(player->socket, "ok\n", 3);
        return 1;
    } 
    else {
        write(player->socket, "ko\n", 3);
        return 0;
    }
}



#include "../includes/server.h"

void assign_player_position(player_t *player, map_t *map) {
    player->x = rand() % map->width;
    player->y = rand() % map->height;
    player->direction = rand() % 4;  // Orientation aléatoire (0 à 3)
}

void game_loop() {
    struct timeval start, end;
    double elapsed;

    while (1) {
        if (game_time.game_started) {
            gettimeofday(&start, NULL);  // Capture le temps de départ
            
            game_time.elapsed_time++;
            log_printf("Temps de jeu écoulé : %d unités\n", game_time.elapsed_time);

            do {
                gettimeofday(&end, NULL);
                elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            } while (elapsed < (1.0 / game_time.time_unit));  // Attendre `1/t` secondes
        } else {
            sleep(1);  // Si le jeu n'a pas commencé, attendre 1 seconde
        }
    }
    return ;
}
#include "../includes/server.h"

const char *direction_names[] = {
    "Nord",
    "Est",
    "Sud",
    "Ouest"
};

void printf_identity(player_t *player) {
    log_printf("Joueur %d, de l'equipe %s, en (%d, %d), direction %s -> ", player->id, player->team_name, player->x, player->y, direction_names[player->direction]);
}

// avance - 7/t - ok
void move_forward(player_t *player, map_t *map) {
    switch (player->direction) {
        case NORTH: player->y = (player->y - 1 + map->height) % map->height; break;
        case EAST:  player->x = (player->x + 1) % map->width; break;
        case SOUTH: player->y = (player->y + 1) % map->height; break;
        case WEST:  player->x = (player->x - 1 + map->width) % map->width; break;
    }
    printf("avance en (%d, %d)\n", player->x, player->y);
}

// droite - 7/t - ok
void turn_right(player_t *player) {
    player->direction = (player->direction + 1) % 4;
    printf("tourne sur sa droite, nouvelle direction : %s\n", direction_names[player->direction]);
}

// droite - 7/t - ok
void turn_left(player_t *player) {
    player->direction = (player->direction - 1 + 4) % 4;
    printf("tourne sur sa gauche, nouvelle direction : %s\n", direction_names[player->direction]);
}



#include "../includes/server.h"

void turn_player(player_t *player, bool left)
{
    int direction_changer = left ? 3 : 1;
    player->direction = (player->direction + direction_changer) % 4;
}

void move_forward(player_t *player, map_t *map)
{
    switch (player->direction)
    {
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
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

bool kick_players(player_t *player, map_t *map, player_t *players[], int max_players)
{
    int coordinate[2];
    bool kicked_one = false;
            
    get_front_coordinate(coordinate, *player, map);
    for (int i = 0; i < max_players; i++)
    {
        if (players[i] != NULL && players[i]->socket != player->socket && players[i]->x == player->x && players[i]->y == player->y)
        {
            players[i]->x = coordinate[0];
            players[i]->y = coordinate[1];
            log_printf_identity(PRINT_INFORMATION, player, "a expulsé le joueur %d de l'equipe %s sur la case [%d, %d]\n", players[i]->socket, players[i]->team_name, players[i]->x, players[i]->y);
            char buffer[24];
            sprintf(buffer, "deplacement %s\n", get_player_direction(players[i]));
            send_message_player(*players[i], buffer);
            kicked_one = true;
        }
    }
    return kicked_one;
}
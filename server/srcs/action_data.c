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

bool kick_players(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players)
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
            send_graphic_player_position(graphic_socket, player);
            kicked_one = true;
        }
    }
    return kicked_one;
}

const resources_t level_up_requirement[] = {
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0},
    {0, 2, 0, 1, 0, 2, 0},
    {0, 1, 1, 2, 0, 1, 0},
    {0, 1, 2, 1, 3, 0, 0},
    {0, 1, 2, 3, 0, 1, 0},
    {0, 2, 2, 2, 2, 2, 1}};

bool can_incantation(player_t *player, map_t *map, player_t *players[], int max_players)
{
    int required_players = player->level - (player->level % 2);
    int good_players = 0;

    // Vérifier la présence des joueurs du même niveau
    for (int i = 0; i < max_players; i++)
    {
        if (players[i] != NULL)
            printf("[%d, %d] - [%d, %d] - [%d, %d]\n", players[i]->x, players[i]->y, player->x, player->y, players[i]->level, player->level);
        if (
            players[i] != NULL &&
            players[i]->x == player->x &&
            players[i]->y == player->y &&
            (players[i]->level == player->level || players[i]->level == (player->level + 1)))
            good_players++;
    }

    if (good_players < required_players)
    {
        print_players(players, max_players);
        log_printf_identity(PRINT_ERROR, player, "a tenté d'incanter mais il n'y a pas assez de joueurs du même niveau (il n'y en a %d joueurs de niveau %d ou %d, il en fallait %d)\n", good_players, player->level, player->level + 1, required_players);
        return false; // Pas assez de joueurs du même niveau
    }

    // Vérifier si les ressources requises sont disponibles
    cell_t *cell = &map->cells[player->y][player->x];
    // printf("AU SOL---- : %d %d %d %d %d %d\n", cell->resources.linemate, cell->resources.deraumere, cell->resources.sibur, cell->resources.mendiane, cell->resources.phiras, cell->resources.thystame);
    // printf("NECESSAIRE : %d %d %d %d %d %d\n", level_up_requirement[player->level].linemate, level_up_requirement[player->level].deraumere, level_up_requirement[player->level].sibur, level_up_requirement[player->level].mendiane, level_up_requirement[player->level].phiras, level_up_requirement[player->level].thystame);
    if (
        cell->resources.linemate >= level_up_requirement[player->level - 1].linemate && cell->resources.deraumere >= level_up_requirement[player->level - 1].deraumere && cell->resources.sibur >= level_up_requirement[player->level - 1].sibur &&
        cell->resources.mendiane >= level_up_requirement[player->level - 1].mendiane && cell->resources.phiras >= level_up_requirement[player->level - 1].phiras &&
        cell->resources.thystame >= level_up_requirement[player->level - 1].thystame)

        return true;

    return false;
}

bool level_up_players(player_t *players[], int max_players)
{
    int i = 0;

    while (players[i] != NULL && i < max_players)
    {
        printf("DO player %d - need levelup[%d]\n", players[i]->socket, players[i]->need_level_up);
        if (players[i]->need_level_up == true)
        {
            players[i]->level++;
            players[i]->need_level_up = false;
            char buffer[BUFFER_SIZE_TINY];
            sprintf(buffer, "level up : %d\n", players[i]->level);
            send_message_player(*players[i], buffer);
            if (players[i]->level == 8)
                return true;
        }
        i++;
    }
    return false;
}

void start_incantation(player_t *player, map_t *map, player_t *players[], int max_players)
{
    int i = 0;

    player->incantation_trigger = true;
    while (players[i] != NULL && i < max_players)
    {
        if (players[i]->x == player->x && players[i]->y == player->y)
        {
            printf("player %d - need levelup\n", players[i]->socket);
            players[i]->need_level_up = true;
        }
        i++;
    }

    cell_t *cell = &map->cells[player->y][player->x];
    int index_requirement = player->level - 1;
    cell->resources.linemate -= level_up_requirement[index_requirement].linemate;
    cell->resources.deraumere -= level_up_requirement[index_requirement].deraumere;
    cell->resources.sibur -= level_up_requirement[index_requirement].sibur;
    cell->resources.mendiane -= level_up_requirement[index_requirement].mendiane;
    cell->resources.phiras -= level_up_requirement[index_requirement].phiras;
    cell->resources.thystame -= level_up_requirement[index_requirement].thystame;
}
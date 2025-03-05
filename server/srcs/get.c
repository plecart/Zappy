#include "../includes/server.h"

static const char *const resource_names[] = {
    NOURRITURE,
    LINEMATE,
    DERAUMERE,
    SIBUR,
    MENDIANE,
    PHIRAS,
    THYSTAME};

char *get_player_direction(player_t *player)
{
    const char *directions[] = {"NORTH", "EAST", "SOUTH", "WEST"};
    return strdup(directions[player->direction]);
}

int get_random_between(int min, int max)
{
    return min + rand() % (max - min + 1);
}

void get_player_inventory(player_t *player, char *buffer, size_t size)
{
    snprintf(buffer, size, "{%s: %d, %s: %d, %s: %d, %s: %d, %s: %d, %s: %d, %s: %d}\n",
             resource_names[0],
             player->inventory.nourriture,
             resource_names[1],
             player->inventory.linemate,
             resource_names[2],
             player->inventory.deraumere,
             resource_names[3],
             player->inventory.sibur,
             resource_names[4],
             player->inventory.mendiane,
             resource_names[5],
             player->inventory.phiras,
             resource_names[6],
             player->inventory.thystame);
}

int get_resource_index(const char *resource_name)
{
    if (resource_name == NULL)
    {
        return -1; // Erreur si le nom de ressource est invalide
    }

    for (int i = 0; i < 7; i++)
    {
        if (strcmp(resource_name, resource_names[i]) == 0)
            return i;
    }
    return -1;
}

int get_visible_cell_count(int level)
{
    return (level + 1) * (level + 1);
}

void get_visible_cells_coordinates(player_t *player, map_t *map, int coordinates[][2])
{
    coordinates[0][0] = player->x;
    coordinates[0][1] = player->y;
    int index = 1;

    for (int depth = 1; depth <= player->level; depth++)
    {
        switch (player->direction)
        {
        case NORTH:
            for (int i = -depth; i <= depth; i++)
            {
                coordinates[index][0] = ((player->x + i) % map->width + map->width) % map->width;
                coordinates[index][1] = ((player->y - depth) % map->height + map->height) % map->height;
                index++;
            }
            break;
        case EAST:
            for (int i = -depth; i <= depth; i++)
            {
                coordinates[index][0] = ((player->x + depth) % map->width + map->width) % map->width;
                coordinates[index][1] = ((player->y + i) % map->height + map->height) % map->height;
                index++;
            }
            break;
        case SOUTH:
            for (int i = depth; i >= -depth; i--)
            {
                coordinates[index][0] = ((player->x + i) % map->width + map->width) % map->width;
                coordinates[index][1] = ((player->y + depth) % map->height + map->height) % map->height;
                index++;
            }
            break;
        case WEST:
            for (int i = depth; i >= -depth; i--)
            {
                coordinates[index][0] = ((player->x - depth) % map->width + map->width) % map->width;
                coordinates[index][1] = ((player->y + i) % map->height + map->height) % map->height;
                index++;
            }
            break;
            break;
        }
    }
}

void get_front_coordinate(int coordinate[2], player_t player, map_t *map)
{
    switch (player.direction)
    {
    case NORTH:
        coordinate[0] = player.x;
        coordinate[1] = ((player.y - 1) % map->height + map->height) % map->height;
        break;
    case EAST:
        coordinate[0] = ((player.x + 1) % map->width + map->width) % map->width;
        coordinate[1] = player.y;
        break;
    case SOUTH:
        coordinate[0] = player.x;
        coordinate[1] = ((player.y + 1) % map->height + map->height) % map->height;
        break;
    case WEST:
        coordinate[0] = ((player.x - 1) % map->width + map->width) % map->width;
        coordinate[1] = player.y;
        break;
    }
}

int get_elements_max_len(int max_players)
{
    int nourriture_len = (strlen(NOURRITURE) + 1) * MAX_NOURRITURE;
    int linemate_len = (strlen(LINEMATE) + 1) * MAX_LINEMATE;
    int deraumere_len = (strlen(DERAUMERE) + 1) * MAX_DERAUMERE;
    int sibur_len = (strlen(SIBUR) + 1) * MAX_SIBUR;
    int mendiane_len = (strlen(MENDIANE) + 1) * MAX_MENDIANE;
    int phiras_len = (strlen(PHIRAS) + 1) * MAX_PHIRAS;
    int thystame_len = (strlen(THYSTAME) + 1) * MAX_THYSTAME;
    int player_len = (strlen(JOUEUR) + 1) * max_players;

    return nourriture_len + linemate_len + deraumere_len + sibur_len + mendiane_len + phiras_len + thystame_len + player_len;
}

void get_elements_from_coordinates(map_t *map, int coordinates[][2], int cell_count, char *buffer, size_t buffer_size, player_t *players[], int max_players)
{
    // 'pos' tracks how many bytes we've placed into 'buffer'.
    // Always ensure buffer[pos] is valid and we never exceed (buffer_size - 1).
    int pos = 0;
    buffer[0] = '\0'; // Start with an empty string

    for (int i = 0; i < cell_count; i++)
    {
        char temp[1024];
        temp[0] = '\0';
        int temp_pos = 0;

        int x = coordinates[i][0];
        int y = coordinates[i][1];
        cell_t *cell = &map->cells[y][x];

        int player_count = 0;
        for (int p = 0; p < max_players; p++)
        {
            if (players[p] != NULL &&
                players[p]->x == x &&
                players[p]->y == y)
            {
                player_count++;
            }
        }

        if (i == 0)
        {
            player_count--;
            if (player_count < 0)
                player_count = 0;
        }

        for (int count = 0; count < player_count; count++)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "joueur ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
                break;
            }
            temp_pos += written;
        }

        for (int r = 0; r < cell->resources.nourriture; r++)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "nourriture ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
                break;
            }
            temp_pos += written;
        }
        for (int r = 0; r < cell->resources.linemate; r++)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "linemate ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
                break;
            }
            temp_pos += written;
        }
        for (int r = 0; r < cell->resources.deraumere; r++)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "deraumere ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
                break;
            }
            temp_pos += written;
        }
        for (int r = 0; r < cell->resources.sibur; r++)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "sibur ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
                break;
            }
            temp_pos += written;
        }
        for (int r = 0; r < cell->resources.mendiane; r++)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "mendiane ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
                break;
            }
            temp_pos += written;
        }
        for (int r = 0; r < cell->resources.phiras; r++)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "phiras ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
                break;
            }
            temp_pos += written;
        }
        for (int r = 0; r < cell->resources.thystame; r++)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "thystame ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
                break;
            }
            temp_pos += written;
        }

        if (player_count <= 0 &&
            cell->resources.nourriture == 0 &&
            cell->resources.linemate == 0 &&
            cell->resources.deraumere == 0 &&
            cell->resources.sibur == 0 &&
            cell->resources.mendiane == 0 &&
            cell->resources.phiras == 0 &&
            cell->resources.thystame == 0)
        {
            int space_left = (int)sizeof(temp) - temp_pos;
            int written = snprintf(temp + temp_pos, space_left, "%s", "vide ");
            if (written < 0 || written >= space_left)
            {
                temp_pos = (int)strlen(temp);
            }
            else
            {
                temp_pos += written;
            }
        }

        if (temp_pos > 0 && temp[temp_pos - 1] == ' ')
        {
            temp[--temp_pos] = '\0';
        }

        {
            int space_left = (int)buffer_size - pos;
            int written;

            if (i > 0)
            {
                written = snprintf(buffer + pos, space_left, ", %s", temp);
            }
            else
            {
                written = snprintf(buffer + pos, space_left, "%s", temp);
            }

            if (written < 0 || written >= space_left)
            {
                pos = (int)strlen(buffer);
                break;
            }
            pos += written;
        }
    }

    if (pos < (int)buffer_size)
    {
        buffer[pos] = '\0';
    }
    else
    {
        buffer[buffer_size - 1] = '\0';
    }
}

int get_sound_direction(player_t *sender, player_t *receiver, map_t *map)
{
    int dx = sender->x - receiver->x;
    int dy = sender->y - receiver->y;

    dx = (dx > map->width / 2) ? dx - map->width : (dx < -map->width / 2) ? dx + map->width
                                                                          : dx;
    dy = (dy > map->height / 2) ? dy - map->height : (dy < -map->height / 2) ? dy + map->height
                                                                             : dy;

    static const int north_directions[3][3] = {
        {8, 1, 2},
        {7, -1, 3},
        {6, 5, 4}};

    static const int east_directions[3][3] = {
        {6, 7, 8},
        {5, -1, 1},
        {4, 3, 2}};

    static const int south_directions[3][3] = {
        {4, 5, 6},
        {3, -1, 7},
        {2, 1, 8}};

    static const int west_directions[3][3] = {
        {2, 3, 4},
        {1, -1, 5},
        {8, 7, 6}};

    if (dx == 0 && dy == 0)
        return 0;

    int index_x = (dx > 0) ? 2 : (dx < 0) ? 0
                                          : 1;
    int index_y = (dy > 0) ? 2 : (dy < 0) ? 0
                                          : 1;

    switch (receiver->direction)
    {
    case NORTH:
        return north_directions[index_y][index_x];
    case EAST:
        return east_directions[index_y][index_x];
    case SOUTH:
        return south_directions[index_y][index_x];
    case WEST:
        return west_directions[index_y][index_x];
    }
    return -1;
}

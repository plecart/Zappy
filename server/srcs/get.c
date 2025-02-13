#include "../includes/server.h"

static const char *const resource_names[] = {
    "nourriture",
    "linemate",
    "deraumere",
    "sibur",
    "mendiane",
    "phiras",
    "thystame"};

char *get_player_direction(player_t *player)
{
    const char *directions[] = {"NORTH", "EAST", "SOUTH", "WEST"};
    return strdup(directions[player->direction]);
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

void get_elements_from_coordinates(map_t *map, int coordinates[][2], int cell_count, char *buffer, player_t *players[], int max_players)
{
    int pos = 0;
    buffer[0] = '\0'; // Initialisation correcte

    for (int i = 0; i < cell_count; i++)
    {
        int x = coordinates[i][0];
        int y = coordinates[i][1];
        cell_t *cell = &map->cells[y][x];

        // Stocker les ressources trouvées dans une case
        char temp[96] = "";
        int temp_pos = 0;
        int player_count = 0;

        if (i != 0)
        {
            while (player_count < max_players && players[player_count] != NULL && players[player_count]->x != x && players[player_count]->y != y)
                player_count++;
            if (player_count != max_players && players[player_count] != NULL)
                temp_pos += snprintf(temp + temp_pos, sizeof(temp) - temp_pos, "joueur ");
        }
        if (cell->resources.nourriture > 0)
            temp_pos += snprintf(temp + temp_pos, sizeof(temp) - temp_pos, "nourriture ");
        if (cell->resources.linemate > 0)
            temp_pos += snprintf(temp + temp_pos, sizeof(temp) - temp_pos, "linemate ");
        if (cell->resources.deraumere > 0)
            temp_pos += snprintf(temp + temp_pos, sizeof(temp) - temp_pos, "deraumere ");
        if (cell->resources.sibur > 0)
            temp_pos += snprintf(temp + temp_pos, sizeof(temp) - temp_pos, "sibur ");
        if (cell->resources.mendiane > 0)
            temp_pos += snprintf(temp + temp_pos, sizeof(temp) - temp_pos, "mendiane ");
        if (cell->resources.phiras > 0)
            temp_pos += snprintf(temp + temp_pos, sizeof(temp) - temp_pos, "phiras ");
        if (cell->resources.thystame > 0)
            temp_pos += snprintf(temp + temp_pos, sizeof(temp) - temp_pos, "thystame ");

        // Supprimer l'espace en trop à la fin de temp
        if (temp_pos > 0 && temp[temp_pos - 1] == ' ')
            temp[temp_pos - 1] = '\0';

        // Vérifier si le buffer a de la place
        if (pos + strlen(temp) + 2 >= (size_t)(96 * cell_count))
            break;

        // Ajouter les ressources au buffer avec une virgule si nécessaire
        if (pos > 0)
            pos += snprintf(buffer + pos, 96 * cell_count - pos, ", ");
        pos += snprintf(buffer + pos, 96 * cell_count - pos, "%s", temp);
    }
}

int get_sound_direction(player_t *sender, player_t *receiver, map_t *map)
{
    int dx = sender->x - receiver->x;
    int dy = sender->y - receiver->y;

    // Gestion du torique (monde wrap-around)
    dx = (dx > map->width / 2) ? dx - map->width : (dx < -map->width / 2) ? dx + map->width : dx;
    dy = (dy > map->height / 2) ? dy - map->height : (dy < -map->height / 2) ? dy + map->height : dy;

    // Matrice de direction relative
    static const int directions[3][3] = {
        {8, 1, 2},
        {7, -1, 3},
        {6, 5, 4}
    };

    if (dx == 0 && dy == 0)
        return 0;
    
    int index_x = (dx > 0) ? 2 : (dx < 0) ? 0 : 1;
    int index_y = (dy > 0) ? 2 : (dy < 0) ? 0 : 1;
    int ret = directions[index_y][index_x];
    
    ret = (6 * receiver->direction + ret) % 8;
    return ret == 0 ? 8 : ret;
}

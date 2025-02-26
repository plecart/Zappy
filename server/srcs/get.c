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

void get_elements_from_coordinates(map_t *map, int coordinates[][2], int cell_count, char *buffer, player_t *players[], int max_players)
{
    int pos = 0;
    buffer[0] = '\0'; // Initialisation correcte

    for (int i = 0; i < cell_count; i++)
    {
        int x = coordinates[i][0];
        int y = coordinates[i][1];
        cell_t *cell = &map->cells[y][x];

        // Calculer la taille nécessaire pour le buffer
        int temp_len = get_elements_max_len(max_players) + 3;
        //printf("temp_len: %d\n", temp_len);
        char *temp = malloc(temp_len * sizeof(char));  // Allocation du buffer temporaire
        temp[0] = '\0'; // Initialisation correcte
        int temp_pos = 0;
        int player_count = 0;

        if (i != 0)
        {
            while (player_count < max_players && players[player_count] != NULL && players[player_count]->x != x && players[player_count]->y != y)
                player_count++;
            if (player_count != max_players && players[player_count] != NULL)
                temp_pos += snprintf(temp + temp_pos, temp_len - temp_pos, "joueur ");
        }

        // Répéter les ressources en fonction de leur quantité
        if (cell->resources.nourriture > 0)
            for (int j = 0; j < cell->resources.nourriture; j++)
                temp_pos += snprintf(temp + temp_pos, temp_len - temp_pos, "nourriture ");
        if (cell->resources.linemate > 0)
            for (int j = 0; j < cell->resources.linemate; j++)
                temp_pos += snprintf(temp + temp_pos, temp_len - temp_pos, "linemate ");
        if (cell->resources.deraumere > 0)
            for (int j = 0; j < cell->resources.deraumere; j++)
                temp_pos += snprintf(temp + temp_pos, temp_len - temp_pos, "deraumere ");
        if (cell->resources.sibur > 0)
            for (int j = 0; j < cell->resources.sibur; j++)
                temp_pos += snprintf(temp + temp_pos, temp_len - temp_pos, "sibur ");
        if (cell->resources.mendiane > 0)
            for (int j = 0; j < cell->resources.mendiane; j++)
                temp_pos += snprintf(temp + temp_pos, temp_len - temp_pos, "mendiane ");
        if (cell->resources.phiras > 0)
            for (int j = 0; j < cell->resources.phiras; j++)
                temp_pos += snprintf(temp + temp_pos, temp_len - temp_pos, "phiras ");
        if (cell->resources.thystame > 0)
            for (int j = 0; j < cell->resources.thystame; j++)
                temp_pos += snprintf(temp + temp_pos, temp_len - temp_pos, "thystame ");

        // Supprimer l'espace en trop à la fin de temp
        if (temp_pos > 0 && temp[temp_pos - 1] == ' ')
            temp[temp_pos - 1] = '\0';

        // Vérifier si le buffer a de la place
        if (pos + strlen(temp) + 2 >= (size_t)(temp_len * cell_count))
            break;

        // Ajouter les ressources au buffer avec une virgule si nécessaire
        if (pos > 0)
            pos += snprintf(buffer + pos, temp_len * cell_count - pos, ", ");
        pos += snprintf(buffer + pos, temp_len * cell_count - pos, "%s", temp);
        
        free(temp);  // Libérer le buffer temporaire
    }
}


int get_sound_direction(player_t *sender, player_t *receiver, map_t *map)
{
    int dx = sender->x - receiver->x;
    int dy = sender->y - receiver->y;

    // Gestion du torique (monde wrap-around)
    dx = (dx > map->width / 2) ? dx - map->width : (dx < -map->width / 2) ? dx + map->width
                                                                          : dx;
    dy = (dy > map->height / 2) ? dy - map->height : (dy < -map->height / 2) ? dy + map->height
                                                                             : dy;

    // Matrice de direction relative
    static const int directions[3][3] = {
        {8, 1, 2},
        {7, -1, 3},
        {6, 5, 4}};

    if (dx == 0 && dy == 0)
        return 0;

    int index_x = (dx > 0) ? 2 : (dx < 0) ? 0
                                          : 1;
    int index_y = (dy > 0) ? 2 : (dy < 0) ? 0
                                          : 1;
    int ret = directions[index_y][index_x];

    ret = (6 * receiver->direction + ret) % 8;
    return ret == 0 ? 8 : ret;
}

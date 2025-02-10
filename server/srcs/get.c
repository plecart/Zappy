#include "../includes/server.h"

static const char *const resource_names[] = {
    "nourriture",
    "linemate",
    "deraumere",
    "sibur",
    "mendiane",
    "phiras",
    "thystame"
};


char*  get_player_direction(player_t *player)
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

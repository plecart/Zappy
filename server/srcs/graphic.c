#include "../includes/server.h"

void send_initial_graphic_data(int graphic_socket, server_config_t *config, map_t *map, player_t *players[], int max_players, egg_t *eggs[], int egg_count)
{
    if (graphic_socket == -1 || map == NULL || players == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    send(graphic_socket, "BIENVENUE\n", 10, 0);
    snprintf(buffer, sizeof(buffer), "msz %d %d\n", config->width, config->height);
    send(graphic_socket, buffer, strlen(buffer), 0);
    snprintf(buffer, sizeof(buffer), "sgt %d\n", config->time_unit);
    send(graphic_socket, buffer, strlen(buffer), 0);

    for (int y = 0; y < config->height; y++)
    {
        for (int x = 0; x < config->width; x++)
        {
            send_graphic_cell(graphic_socket, map->cells[y][x], x, y);
        }
    }

    for (int i = 0; i < config->team_count; i++)
    {
        snprintf(buffer, sizeof(buffer), "tna %s\n", config->teams[i]);
        send(graphic_socket, buffer, strlen(buffer), 0);
    }

    for (int i = 0; i < max_players; i++)
    {
        if (players[i] != NULL)
        {
            snprintf(buffer, sizeof(buffer), "pnw #%d %d %d %d %d %s\n",
                     players[i]->socket, players[i]->x, players[i]->y, players[i]->direction, players[i]->level, players[i]->team_name);
            send(graphic_socket, buffer, strlen(buffer), 0);
        }
    }

    for (int i = 0; i < egg_count; i++)
    {
        snprintf(buffer, sizeof(buffer), "enw #%d #%d %d %d\n",
                 i, eggs[i]->mother_socket, eggs[i]->x, eggs[i]->y);
        send(graphic_socket, buffer, strlen(buffer), 0);
    }
}

void send_graphic_cell(int graphic_socket, cell_t cell, int x, int y)
{
    if (graphic_socket == -1)
    {
        return;
    }

    char buffer[BUFFER_SIZE];

    snprintf(buffer, sizeof(buffer), "bct %d %d %d %d %d %d %d %d %d\n", x, y,
             cell.resources.nourriture,
             cell.resources.linemate,
             cell.resources.deraumere,
             cell.resources.sibur,
             cell.resources.mendiane,
             cell.resources.phiras,
             cell.resources.thystame);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_new_player(int graphic_socket, player_t *player, bool from_egg, int egg_id)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];

    if (from_egg)
    {
        snprintf(buffer, sizeof(buffer), "ebo #%d\n", egg_id);
        send(graphic_socket, buffer, strlen(buffer), 0);
    }

    snprintf(buffer, sizeof(buffer), "pnw #%d %d %d %d %d %s\n",
             player->socket,
             player->x,
             player->y,
             player->direction,
             player->level,
             player->team_name);

    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_player_position(int graphic_socket, player_t *player)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "ppo #%d %d %d %d\n",
             player->socket,
             player->x,
             player->y,
             player->direction);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_player_inventory(int graphic_socket, player_t *player)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "pin #%d %d %d %d %d %d %d %d %d %d\n",
             player->socket,
             player->x, player->y,
             player->inventory.nourriture,
             player->inventory.linemate,
             player->inventory.deraumere,
             player->inventory.sibur,
             player->inventory.mendiane,
             player->inventory.phiras,
             player->inventory.thystame);

    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_player_resources(int graphic_socket, player_t *player, map_t *map, bool take, int resource_index)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s #%d %d\n", take ? "pgt" : "pdr", player->socket, resource_index);
    send(graphic_socket, buffer, strlen(buffer), 0);
    send_graphic_player_inventory(graphic_socket, player);
    send_graphic_cell(graphic_socket, map->cells[player->y][player->x], player->x, player->y);
}

void send_graphic_expulse(int graphic_socket, player_t *player)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "pex #%d\n", player->socket);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_broadcast(int graphic_socket, player_t *player, const char *message)
{
    if (graphic_socket == -1 || player == NULL || message == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];

    snprintf(buffer, sizeof(buffer), "pbc #%d %s\n", player->socket, message);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_incantation_start(int graphic_socket, player_t *player, player_t *players[], int max_players)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    int pos = snprintf(buffer, sizeof(buffer), "pic %d %d %d", player->x, player->y, player->level);

    for (int i = 0; i < max_players; i++)
    {
        if (players[i] != NULL && players[i]->x == player->x && players[i]->y == player->y)
        {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, " #%d", players[i]->socket);
        }
    }

    snprintf(buffer + pos, sizeof(buffer) - pos, "\n");
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_incantation_end(int graphic_socket, player_t *player, map_t *map, bool success)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];

    snprintf(buffer, sizeof(buffer), "pie %d %d %d\n", player->x, player->y, success ? 1 : 0);
    send(graphic_socket, buffer, strlen(buffer), 0);

    if (success)
    {
        send_graphic_cell(graphic_socket, map->cells[player->y][player->x], player->x, player->y);
    }
}

void send_graphic_player_level(int graphic_socket, player_t *player)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];

    snprintf(buffer, sizeof(buffer), "plv #%d %d\n", player->socket, player->level);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_fork(int graphic_socket, player_t *player)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "pfk #%d\n", player->socket);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_egg_created(int graphic_socket, int egg_id, egg_t *egg)
{
    if (graphic_socket == -1 || egg == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "enw #%d #%d %d %d\n", egg_id, egg->mother_socket, egg->x, egg->y);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_egg_hatched(int graphic_socket, int egg_id)
{
    if (graphic_socket == -1)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "eht #%d\n", egg_id);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_player_death(int graphic_socket, player_t *player)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "pdi #%d\n", player->socket);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_game_end(int graphic_socket, const char *winning_team)
{
    if (graphic_socket == -1 || winning_team == NULL)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "seg %s\n", winning_team);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

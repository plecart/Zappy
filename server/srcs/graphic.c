#include "../includes/server.h"

void send_initial_graphic_data(int graphic_socket, server_config_t *config, map_t *map, player_t *players[], int max_players, egg_t *eggs[], int egg_count)
{

    if (graphic_socket == -1 || map == NULL || players == NULL)
        return;

    char buffer[BUFFER_SIZE];

    send(graphic_socket, "BIENVENUE\n", 10, 0);
    // Taille de la carte
    snprintf(buffer, sizeof(buffer), "msz %d %d\n", config->width, config->height);
    send(graphic_socket, buffer, strlen(buffer), 0);

    // Unité de temps
    snprintf(buffer, sizeof(buffer), "sgt %d\n", config->time_unit);
    send(graphic_socket, buffer, strlen(buffer), 0);

    // Contenu de la carte
    for (int y = 0; y < config->height; y++)
    {
        for (int x = 0; x < config->width; x++)
        {
            snprintf(buffer, sizeof(buffer), "bct %d %d %d %d %d %d %d %d %d\n", x, y,
                     map->cells[y][x].resources.nourriture,
                     map->cells[y][x].resources.linemate,
                     map->cells[y][x].resources.deraumere,
                     map->cells[y][x].resources.sibur,
                     map->cells[y][x].resources.mendiane,
                     map->cells[y][x].resources.phiras,
                     map->cells[y][x].resources.thystame);
            send(graphic_socket, buffer, strlen(buffer), 0);
        }
    }

    // Noms des équipes
    for (int i = 0; i < config->team_count; i++)
    {
        snprintf(buffer, sizeof(buffer), "tna %s\n", config->teams[i]);
        send(graphic_socket, buffer, strlen(buffer), 0);
    }

    // Joueurs connectés
    for (int i = 0; i < max_players; i++)
    {
        if (players[i] != NULL)
        {
            snprintf(buffer, sizeof(buffer), "pnw #%d %d %d %d %d %s\n",
                     players[i]->socket, players[i]->x, players[i]->y, players[i]->direction, players[i]->level, players[i]->team_name);
            send(graphic_socket, buffer, strlen(buffer), 0);
        }
    }

    // Œufs présents
    for (int i = 0; i < egg_count; i++)
    {
        snprintf(buffer, sizeof(buffer), "enw #%d #%d %d %d\n",
                 i, eggs[i]->mother_socket, eggs[i]->x, eggs[i]->y);
        send(graphic_socket, buffer, strlen(buffer), 0);
    }
}

void send_graphic_new_player(int graphic_socket, player_t *player, bool from_egg, int egg_id)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return; // Pas de client graphique connecté ou joueur invalide
    }

    char buffer[BUFFER_SIZE];

    if (from_egg)
    {
        snprintf(buffer, sizeof(buffer), "ebo #%d\n", egg_id);
        send(graphic_socket, buffer, strlen(buffer), 0);
    }
    // 📌 Envoi de la commande pnw (Connexion d’un joueur)
    snprintf(buffer, sizeof(buffer), "pnw #%d %d %d %d %d %s\n",
             player->socket,     // ID unique du joueur (socket)
             player->x,          // Position X
             player->y,          // Position Y
             player->direction,  // Orientation (1 = Nord, 2 = Est, 3 = Sud, 4 = Ouest)
             player->level,      // Niveau du joueur
             player->team_name); // Nom de l’équipe

    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_player_position(int graphic_socket, player_t *player)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return; // Aucun client graphique ou joueur invalide
    }

    char buffer[BUFFER_SIZE];

    snprintf(buffer, sizeof(buffer), "ppo #%d %d %d %d\n",
             player->socket,     // ID du joueur
             player->x,          // Nouvelle position X
             player->y,          // Nouvelle position Y
             player->direction); // Orientation (1 = Nord, 2 = Est, 3 = Sud, 4 = Ouest)

    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_player_resources(int graphic_socket, player_t *player, map_t *map, bool take, int resource_index)
{
    if (graphic_socket == -1 || player == NULL)
    {
        return; // Aucun client graphique ou joueur invalide
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s #%d %d\n", take ? "pgt" : "pdr", player->socket, resource_index);
    send(graphic_socket, buffer, strlen(buffer), 0);

    snprintf(buffer, sizeof(buffer), "pin #%d %d %d %d %d %d %d %d %d %d\n",
             player->socket,       // ID du joueur
             player->x, player->y, // Position
             player->inventory.nourriture,
             player->inventory.linemate,
             player->inventory.deraumere,
             player->inventory.sibur,
             player->inventory.mendiane,
             player->inventory.phiras,
             player->inventory.thystame);

    send(graphic_socket, buffer, strlen(buffer), 0);

    int x = player->x;
    int y = player->y;
    snprintf(buffer, sizeof(buffer), "bct %d %d %d %d %d %d %d %d %d\n",
             x, y,
             map->cells[y][x].resources.nourriture,
             map->cells[y][x].resources.linemate,
             map->cells[y][x].resources.deraumere,
             map->cells[y][x].resources.sibur,
             map->cells[y][x].resources.mendiane,
             map->cells[y][x].resources.phiras,
             map->cells[y][x].resources.thystame);

    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_expulse(int graphic_socket, player_t *player)
{
    if (graphic_socket == -1 || player == NULL) {
        return;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "pex #%d\n", player->socket);
    send(graphic_socket, buffer, strlen(buffer), 0);
}

void send_graphic_broadcast(int graphic_socket, player_t *player, const char *message)
{
    if (graphic_socket == -1 || player == NULL || message == NULL) {
        return;
    }

    char buffer[BUFFER_SIZE];

    snprintf(buffer, sizeof(buffer), "pbc #%d %s\n", player->socket, message);
    send(graphic_socket, buffer, strlen(buffer), 0);
}


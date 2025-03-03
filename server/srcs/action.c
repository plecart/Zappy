#include "../includes/server.h"

bool execute_player_action(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players, egg_t *eggs[], int *egg_count)
{
    if (player->current_execution_time > 1)
    {
        player->current_execution_time--;
        return false;
    }
    if (player->incantation_trigger == true)
    {
        printf("INCANTATION TR IGGER\n");
        player->incantation_trigger = false;
        send_graphic_incantation_end(graphic_socket, player, map, can_incantation(player, map, players, max_players));
        if (level_up_players(players, player, max_players) == true)
            return true;
    }
    if (player->action_count == 0)
        return false;
    player->current_execution_time = action_switch(graphic_socket, player, player->actions[0], map, players, max_players, eggs, egg_count);
    free(player->actions[0]);
    for (int i = 1; i < player->action_count; i++)
    {
        player->actions[i - 1] = player->actions[i];
    }
    player->actions[player->action_count - 1] = NULL;
    player->action_count--;
    return false;
}

int action_switch(int graphic_socket, player_t *player, char *action, map_t *map, player_t *players[], int max_players, egg_t *eggs[], int *egg_count)
{

    log_printf_identity(PRINT_RECEIVE, player, "a [serveur] -> %s\n", action);
    if (strcmp(action, "avance") == 0)
        return action_move_forward(graphic_socket, player, map);
    if (strcmp(action, "droite") == 0)
        return action_turn(graphic_socket, player, false);
    if (strcmp(action, "gauche") == 0)
        return action_turn(graphic_socket, player, true);
    if (strcmp(action, "voir") == 0)
        return action_see(player, map, players, max_players);
    if (strcmp(action, "inventaire") == 0)
        return action_inventory(player);
    if (strncmp(action, "prend", 5) == 0)
        return action_take(graphic_socket, player, map, action);
    if (strncmp(action, "pose", 4) == 0)
        return action_put(graphic_socket, player, map, action);
    if (strcmp(action, "expulse") == 0)
        return action_kick(graphic_socket, player, map, players, max_players);
    if (strncmp(action, "broadcast", 9) == 0)
        return action_broadcast(graphic_socket, player, map, players, max_players, action);
    if (strcmp(action, "incantation") == 0)
        return action_incantation(graphic_socket, player, map, players, max_players);
    if (strcmp(action, "fork") == 0)
        return action_lay_egg(graphic_socket, player, eggs, egg_count);

    log_printf_identity(PRINT_ERROR, player, "a envoye une commande inconnue: %s\n", action);
    send_message_player(*player, "Unknown command\n");
    return 0;
}

int action_move_forward(int graphic_socket, player_t *player, map_t *map)
{
    move_forward(player, map);
    log_printf_identity(PRINT_INFORMATION, player, "a avancé en direction de %s, en [%d, %d]\n", get_player_direction(player), player->x, player->y);
    send_message_player(*player, "ok\n");
    send_graphic_player_position(graphic_socket, player);
    return 7;
}

int action_turn(int graphic_socket, player_t *player, bool left)
{
    turn_player(player, left);
    log_printf_identity(PRINT_INFORMATION, player, "tourne à %s, nouvelle direction: %s\n", left ? "gauche" : "droite", get_player_direction(player));
    send_message_player(*player, "ok\n");
    send_graphic_player_position(graphic_socket, player);
    return 7;
}

int action_see(player_t *player, map_t *map, player_t *players[], int max_players)
{
    int visible_cell_count = get_visible_cell_count(player->level);
    int coordinates[visible_cell_count][2];
    //printf("1\n");
    get_visible_cells_coordinates(player, map, coordinates);
    //printf("2\n");
    int elements_max_len = get_elements_max_len(max_players);
   /// printf("3333\n");
    int buffer_len = ((elements_max_len + 1) * visible_cell_count + 1);
    char *buffer = malloc(buffer_len * sizeof(char));
    get_elements_from_coordinates(map, coordinates, visible_cell_count, buffer, players, max_players);
    //printf("3\n");
    char *package = malloc((buffer_len + 4) * sizeof(char));
    snprintf(package, buffer_len + 4, "{%s}\n", buffer);
    free(buffer);
    buffer = NULL;
    log_printf_identity(PRINT_INFORMATION, player, "a demandé ce qu'il voiyait depuis [%d, %d] en direction du %s\n", player->x, player->y, get_player_direction(player));
    send_message_player(*player, package);
    free(package);
    package = NULL;
    return 7;
}

int action_inventory(player_t *player)
{
    char buffer[BUFFER_SIZE_SMALL];
    get_player_inventory(player, buffer, sizeof(buffer));
    log_printf_identity(PRINT_INFORMATION, player, "a demandé son inventaire\n");
    send_message_player(*player, buffer);
    return 1;
}

int action_take(int graphic_socket, player_t *player, map_t *map, const char *action)
{
    int resource_index;
    char *object = strchr(action, ' ') + 1;

    if (object == NULL || (resource_index = get_resource_index(object)) == -1)
    {
        log_printf_identity(PRINT_ERROR, player, "a envoyé une commande invalide: %s\n", action);
        send_message_player(*player, "ko\n");
        return 0;
    }

    printf("RESSOURCE INDEX: %d\n", resource_index);
    int count = ((int *)&map->cells[player->y][player->x].resources)[resource_index];

    printf("OGT[%d][%d][%d][%d][%d][%d][%d]\n",
        map->cells[player->y][player->x].resources.nourriture,
        map->cells[player->y][player->x].resources.linemate,
        map->cells[player->y][player->x].resources.deraumere, 
        map->cells[player->y][player->x].resources.sibur, 
        map->cells[player->y][player->x].resources.mendiane, 
        map->cells[player->y][player->x].resources.phiras, 
        map->cells[player->y][player->x].resources.thystame
    );
    
    if (count <= 0)
    {
        
        log_printf_identity(PRINT_ERROR, player, "a demandé à prendre l'objet \"%s\", il n'y en a pas en [%d, %d]\n", object, player->x, player->y);
        send_message_player(*player, "ko\n");
        return 0;
    }
    ((int *)&map->cells[player->y][player->x].resources)[resource_index]--;
    ((int *)&player->inventory)[resource_index]++;
    send_message_player(*player, "ok\n");
    log_printf_identity(PRINT_INFORMATION, player, "a prit l'objet \"%s\", il y en reste %d en [%d, %d], la ou il se trouve (il en a possed %d dans son inventaire)\n", object, ((int *)&map->cells[player->y][player->x].resources)[resource_index], player->x, player->y, ((int *)&player->inventory)[resource_index]);
    send_graphic_player_resources(graphic_socket, player, map, true, resource_index);
    return 7;
}

int action_put(int graphic_socket, player_t *player, map_t *map, const char *action)
{
    int resource_index;
    char *object = strchr(action, ' ') + 1;

    if (object == NULL || (resource_index = get_resource_index(object)) == -1)
    {
        log_printf_identity(PRINT_ERROR, player, "a envoyé une commande invalide: %s\n", action);
        send_message_player(*player, "ko\n");
        return 0;
    }

    int count = ((int *)&player->inventory)[resource_index];
    if (count <= 0)
    {
        log_printf_identity(PRINT_ERROR, player, "a demandé à deposer l'objet \"%s\" en [%d,%d], il n'en a pas\n", object, player->x, player->y);
        send_message_player(*player, "ko\n");
        return 0;
    }
    ((int *)&player->inventory)[resource_index]--;
    ((int *)&map->cells[player->y][player->x].resources)[resource_index]++;
    send_message_player(*player, "ok\n");
    log_printf_identity(PRINT_INFORMATION, player, "a deposer l'objet \"%s\", il y en a miantenant %d en [%d, %d], la ou il se trouve (il en possede %d dans son inventaire)\n", object, ((int *)&map->cells[player->y][player->x].resources)[resource_index], player->x, player->y, ((int *)&player->inventory)[resource_index]);
    send_graphic_player_resources(graphic_socket, player, map, false, resource_index);
    return 7;
}

int action_kick(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players)
{
    send_graphic_expulse(graphic_socket, player);
    bool did_kicked = kick_players(graphic_socket, player, map, players, max_players);
    send_message_player(*player, did_kicked ? "ok\n" : "ko\n");
    return 7;
}

int action_broadcast(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players, const char *action)
{
    int player_count = 0;
    char *message = strchr(action, ' ') + 1;
    int cell_source = -1;
    int message_len = strlen(message);

    if (message == NULL)
    {
        log_printf_identity(PRINT_ERROR, player, "a envoyé une commande invalide: %s\n", action);
        send_message_player(*player, "ko\n");
        return 0;
    }
    while (players[player_count] != NULL && player_count < max_players)
    {
        if (players[player_count]->socket != player->socket)
        {
            if ((cell_source = get_sound_direction(player, players[player_count], map)) == -1)
            {
                log_printf_identity(PRINT_ERROR, players[player_count], "ne sait pas d'ou vient le message: %s\n", action);
            }
            else
            {
                char buffer[message_len + 24];
                sprintf(buffer, "message %d, %s\n", cell_source, message);
                send_message_player(*players[player_count], buffer);
            }
        }
        player_count++;
    }
    send_message_player(*player, "ok\n");
    send_graphic_broadcast(graphic_socket, player, message);
    return 7;
}

int action_incantation(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players)
{
    if (can_incantation(player, map, players, max_players))
    {
        start_incantation(player, map, players, max_players);
        char buffer[BUFFER_SIZE_TINY];
        sprintf(buffer, "elevation en cours niveau actuel : %d\n", player->level);
        send_message_player(*player, buffer);
        send_graphic_incantation_start(graphic_socket, player, players, max_players);
        return 300;
    }
    send_message_player(*player, "ko\n");
    return 7;
}

int action_lay_egg(int graphic_socket, player_t *player, egg_t *eggs[], int *egg_count)
{
    egg_t *new_egg = malloc(sizeof(egg_t)); // ✅ Allouer la mémoire correctement
    if (!new_egg)
    {
        log_printf(PRINT_ERROR, "Échec de l'allocation mémoire pour un œuf\n");
        return 0;
    }

    static int egg_id = 0;

    strcpy(new_egg->team_name, player->team_name);
    new_egg->mother_socket = player->socket;
    new_egg->x = player->x;
    new_egg->y = player->y;
    new_egg->time_before_spawn = 42;  // remettre 42
    new_egg->time_before_hatch = 600; // remettre 600
    new_egg->id = egg_id++;

    add_egg(eggs, egg_count, new_egg);

    // ✅ Debugging : Afficher l'état de l'œuf ajouté
    // printf("DEBUG: Œuf ajouté - Team: %s, Pos: (%d, %d), Spawn: %d, Hatch: %d, egg_count: %d\n",
    //        new_egg->team_name, new_egg->x, new_egg->y, new_egg->time_before_spawn, new_egg->time_before_hatch, *egg_count);

    log_printf_identity(PRINT_INFORMATION, player, "commence à pondre un œuf en [%d, %d]\n", player->x, player->y);
    send_message_player(*player, "ok\n");
    send_graphic_fork(graphic_socket, player);
    return 42; // a remettre
}

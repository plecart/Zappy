#ifndef SERVER_H_
#define SERVER_H_

#include "../../tools/includes/tools.h"

typedef struct  server_config_s
{
    int         port;
    int         width;
    int         height;
    char        **teams;
    int         team_count;
    int         clients_per_team;
    int         time_unit;
}               server_config_t;

typedef struct  resources_s
{
    int         nourriture;
    int         linemate;
    int         deraumere;
    int         sibur;
    int         mendiane;
    int         phiras;
    int         thystame;
}               resources_t;

typedef struct  cell_s
{
    resources_t resources;
}               cell_t;

typedef struct  map_s
{
    int         width;
    int         height;
    cell_t      **cells;
}               map_t;

typedef struct  game_time_s {
    int         elapsed_time;
    int         time_unit;
    bool        game_started;
}               game_time_t;

typedef enum
{
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
}               direction_t;

typedef struct  player_s {
    int         id;
    int         x;
    int         y;
    direction_t direction;
    int         socket;
    char        *team_name;
    double      action_time_remaining;  // Temps restant avant d'exécuter une nouvelle action
    int         command_count;  // Nombre de commandes en attente
    char        command_queue[10][BUFFER_SIZE];  // File d'attente des commandes (max 10)
}               player_t;

extern          game_time_t game_time;
extern          player_t *players;
extern          int player_count;

void            print_server_usage(char *prog_name);
server_config_t parse_server_arguments(int argc, char *argv[]);
void            print_server_config(server_config_t config);

void            start_server(server_config_t config);
int             init_server(int port);
void            accept_new_client(int server_socket, int *client_sockets, int max_clients, map_t *map);
void            handle_client_messages(int *client_sockets, int max_clients, fd_set *read_fds);
void            send_message_to_all_clients(const char *message, int *client_sockets, int max_clients);

map_t           *create_map(int width, int height);
void            populate_map(map_t *map);
void            print_map(map_t *map);
void            free_map(map_t *map);

void            assign_player_position(player_t *player, map_t *map);
void            *game_loop(void *arg);
void            enqueue_command(player_t *player, const char *command);
void            process_player_command(player_t *player, map_t *map, double delta_time);
double          execute_player_action(player_t *player, const char *command, map_t *map);

void            printf_identity(player_t *player);
void            move_forward(player_t *player, map_t *map);
void            turn_right(player_t *player);
void            turn_left(player_t *player);

#endif 
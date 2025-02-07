#ifndef SERVER_H_
#define SERVER_H_

#include "../../tools/includes/tools.h"

#define MAX_ACTIONS 10

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

typedef struct player_s
{
    int         socket;
    char        team_name[BUFFER_SIZE];
    int         x;
    int         y;
    char        *actions[MAX_ACTIONS];
    int         action_count;
    int         current_execution_time;
}               player_t;

void            print_server_usage(char *prog_name);
server_config_t parse_server_arguments(int argc, char *argv[]);
void            print_server_config(server_config_t config);

void            start_server(server_config_t config);
int             init_server(int port);
void            accept_new_client(int server_socket, player_t *players[], int max_players, server_config_t *config);
void handle_client_messages(player_t *players[], int max_players, fd_set *read_fds) ;
void            send_message_to_all_clients(const char *message, int *client_sockets, int max_clients);

map_t           *create_map(int width, int height);
void            populate_map(map_t *map);
void            print_map(map_t *map);
void            free_map(map_t *map);

void            free_players(player_t *players[], int max_clients);
void            print_players(player_t *players[], int max_players);
void            add_action_to_player(player_t *player, const char *action);

void            execute_player_action(player_t *player);



#endif 
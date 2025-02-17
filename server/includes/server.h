#ifndef SERVER_H_
#define SERVER_H_

#include "../../tools/includes/tools.h"

#define MAX_ACTIONS 10

typedef struct server_config_s
{
    int port;
    int width;
    int height;
    char **teams;
    int team_count;
    int clients_per_team;
    int time_unit;
} server_config_t;

typedef struct resources_s
{
    int nourriture;
    int linemate;
    int deraumere;
    int sibur;
    int mendiane;
    int phiras;
    int thystame;
} resources_t;

typedef struct cell_s
{
    resources_t resources;
} cell_t;

typedef struct egg_s
{
    char team_name[BUFFER_SIZE_MEDIUM];
    int mother_socket;
    int x;
    int y;
    int time_before_spawn;
    int time_before_hatch;
}               egg_t;

typedef struct map_s
{
    int width;
    int height;
    cell_t **cells;
} map_t;

typedef enum
{
    NORTH,
    EAST,
    SOUTH,
    WEST
} Direction;

typedef struct player_s
{
    int socket;
    char team_name[BUFFER_SIZE_MEDIUM];
    Direction direction;
    int x;
    int y;
    char *actions[MAX_ACTIONS];
    int action_count;
    int current_execution_time;
    resources_t inventory;
    bool need_level_up;
    bool incantation_trigger;
    int level;
    int life_cycle;
} player_t;

void print_server_usage(char *prog_name);
server_config_t parse_server_arguments(int argc, char *argv[]);
void print_server_config(server_config_t config);

void start_server(server_config_t config);
int init_server(int port);
void accept_new_client(int server_socket, player_t *players[], egg_t *eggs[], int *egg_count, int max_clients, server_config_t *config, map_t *map, int *graphic_socket, bool *game_started);
void handle_client_messages(player_t *players[], int max_players, fd_set *read_fds);

void send_message_egg(egg_t egg, const char *message);
void send_message_player(player_t player, const char *message);
void server_send_message(int socket, const char *message, char *team_name);

map_t *create_map(int width, int height);
void populate_map(map_t *map);
void print_map(map_t *map);
void free_map(map_t *map);

void free_player(player_t *player);
void free_players(player_t *players[]);
void print_players(player_t *players[], int max_players);
bool player_eat(int graphic_socket, player_t *player);
void log_printf_identity(print_type type, player_t *player, const char *format, ...);
void add_action_to_player(player_t *player, const char *action);

void turn_player(player_t *player, bool left);
void move_forward(player_t *player, map_t *map);
bool kick_players(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players);
bool can_incantation(player_t *player, map_t *map, player_t *players[], int max_players);
void start_incantation(player_t *player, map_t *map, player_t *players[], int max_players);
void level_up_players(player_t *players[], int max_players);

char *get_player_direction(player_t *player);
void get_player_inventory(player_t *player, char *buffer, size_t size);
int get_resource_index(const char *resource_name);
int get_visible_cell_count(int level);
void get_visible_cells_coordinates(player_t *player, map_t *map, int coordinates[][2]);
void get_elements_from_coordinates(map_t *map, int coordinates[][2], int cell_count, char *buffer, player_t *players[], int max_players);
void get_front_coordinate(int coordinate[2], player_t player, map_t *map);
int get_sound_direction(player_t *sender, player_t *receiver, map_t *map);

void execute_player_action(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players, egg_t *eggs[], int *egg_count);
int action_switch(int graphic_socket, player_t *player, char *action, map_t *map, player_t *players[], int max_players, egg_t *eggs[], int *egg_count);
int action_move_forward(int graphic_socket, player_t *player, map_t *map);
int action_turn(int graphic_socket, player_t *player, bool left);
int action_see(player_t *player, map_t *map, player_t *players[], int max_players);
int action_inventory(player_t *player);
int action_take(int graphic_socket, player_t *player, map_t *map, const char *action);
int action_put(int graphic_socket, player_t *player, map_t *map, const char *action);
int action_kick(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players);
int action_broadcast(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players, const char *action);
int action_incantation(int graphic_socket, player_t *player, map_t *map, player_t *players[], int max_players);
int action_lay_egg(int graphic_socket, player_t *player, egg_t *eggs[], int *egg_count);

void add_egg(egg_t *eggs[], int *egg_count, egg_t *new_egg);
void remove_egg(egg_t *eggs[], int *egg_count, const char *team_name, int x, int y);
void add_egg_cycle(int graphic_socket, egg_t *eggs[], int egg_count);

void send_initial_graphic_data(int graphic_socket, server_config_t *config, map_t *map, player_t *players[], int max_players, egg_t *eggs[], int egg_count);
void send_graphic_cell(int graphic_socket, cell_t cell, int x, int y);
void send_graphic_new_player(int graphic_socket, player_t *player, bool from_egg, int egg_id);
void send_graphic_player_position(int graphic_socket, player_t *player);
void send_graphic_player_resources(int graphic_socket, player_t *player, map_t *map, bool take, int resource_index);
void send_graphic_expulse(int graphic_socket, player_t *player);
void send_graphic_broadcast(int graphic_socket, player_t *player, const char *message);
void send_graphic_incantation_start(int graphic_socket, player_t *player, player_t *players[], int max_players);
void send_graphic_incantation_end(int graphic_socket, player_t *player, map_t *map, bool success);
void send_graphic_player_level(int graphic_socket, player_t *player);
void send_graphic_fork(int graphic_socket, player_t *player);
void send_graphic_egg_created(int graphic_socket, int egg_id, egg_t *egg);
void send_graphic_egg_hatched(int graphic_socket, int egg_id);
void send_graphic_player_death(int graphic_socket, player_t *player);
void send_graphic_game_end(int graphic_socket, const char *winning_team);






#endif
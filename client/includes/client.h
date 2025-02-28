#ifndef CLIENT_H
#define CLIENT_H

#include "../../tools/includes/tools.h"

#define MAX_RESPONSES_COMMANDS 128
#define RESPONSES_TAB *responses[MAX_RESPONSES_COMMANDS]

extern const char *const resource_names[];
extern const int  resource_total_needed[];

typedef enum server_response_type_e
{
    SERVER_RESPONSE_OK_KO,
    SERVER_RESPONSE_OBJECT,
    SERVER_RESPONSE_BEACON,
    SERVER_RESPONSE_INCANTATION,
} server_response_type_t;

typedef struct client_config_s
{
    char *team_name;
    char *hostname;
    int port;
} client_config_t;

client_config_t parse_client_arguments(int argc, char *argv[]);
void print_client_usage(char *prog_name);
void print_client_config(client_config_t config);

void start_client(client_config_t config, bool is_slave);
int connect_to_server(client_config_t config);
int receive_server_response(int sock, char RESPONSES_TAB, int response_count);
void send_message(int sock, const char *message);

int get_response_index(char RESPONSES_TAB, server_response_type_t type, int response_count);
void delete_response(char RESPONSES_TAB, int *response_count, int index);
void filter_responses(char RESPONSES_TAB, int *response_count, client_config_t config, bool is_slave);
void filter_slaves_extra_responses(char RESPONSES_TAB, int *response_count);
void print_responses(char RESPONSES_TAB, int response_count);

void brain(char RESPONSES_TAB, int response_count, int sock, client_config_t config);
void scan_for_resource(int sock, char RESPONSES_TAB, int *response_count, char *resource_name);
int execute_action(int sock, char *action, char RESPONSES_TAB, int *response_count, server_response_type_t response_type, bool delete);
int look(int sock, char RESPONSES_TAB, int *response_count, char *resource_name, int *player_level);
void move_next_spot(int sock, int player_level, char RESPONSES_TAB, int *response_count);
void go_to_cell(int resource_position, int sock, char RESPONSES_TAB, int *response_count);
void broadcast_mission(int sock, char RESPONSES_TAB, int *response_count, char *team_name);
int inventory(int sock, char RESPONSES_TAB, int *response_count, char *resource_name);
int how_much_players(int sock, char RESPONSES_TAB, int *response_count);
void broadcast_beacon(int sock, char RESPONSES_TAB, int *response_count, char *team_name);
bool enough_resources(int sock, char RESPONSES_TAB, int *response_count, int required_resources[6]);


bool is_coordinate(char *str);
bool is_broadcast_team(const char *str, const char *team_name);
int get_mission(const char *str, const char *team_name, char *mission);
int get_view(char responses[BUFFER_SIZE_SMALL], char cells[8 * 8 + 3][BUFFER_SIZE]);
int get_resource_position(char cells[8 * 8 + 3][BUFFER_SIZE], int cells_number, char *resource_name);
bool did_egg_hatched(char RESPONSES_TAB, int *response_count);
int get_item_amount(char *inventory, char *resource_name);
int get_index_ressource(const char *resource);
int is_slave_ready(char RESPONSES_TAB, int *response_count);
int get_players_count(const char *str);
int get_message_direction(const char *str);
bool is_item_enough(const char *view, const int required_resources[6])
;


void start_slave(client_config_t config);
void slave(char RESPONSES_TAB, int response_count, int sock, client_config_t config);
void one_step_to_master(int direction, int sock, char RESPONSES_TAB, int *response_count);
#endif
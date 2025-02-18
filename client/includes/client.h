#ifndef CLIENT_H
#define CLIENT_H

#include "../../tools/includes/tools.h"

#define MAX_RESPONSES_COMMANDS 15
#define RESPONSES_TAB responses[MAX_RESPONSES_COMMANDS][BUFFER_SIZE_SMALL]

typedef enum server_response_type_e
{
    SERVER_RESPONSE_OK_KO,
    SERVER_RESPONSE_OBJECT,
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

void start_client(client_config_t config);
int connect_to_server(client_config_t config);
int receive_server_response(int sock, char RESPONSES_TAB, int response_count);
void send_message(int sock, const char *message);

int get_response_index(char RESPONSES_TAB, server_response_type_t type, int response_count);
void delete_response(char RESPONSES_TAB, int *response_count, int index);

void brain(int sock);
void scan_for_resource(int sock, char RESPONSES_TAB, int *response_count, char *resource_name);
int get_view(char responses[BUFFER_SIZE_SMALL], char cells[8 * 8][BUFFER_SIZE]);
#endif
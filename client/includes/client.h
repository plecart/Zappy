#ifndef CLIENT_H
#define CLIENT_H

#include "../../tools/includes/tools.h"

typedef struct  client_config_s
{
    char        *team_name;
    char        *hostname;
    int         port;
}               client_config_t;

client_config_t parse_client_arguments(int argc, char *argv[]);
void            print_client_usage(char *prog_name);
void            print_client_config(client_config_t config);

void            start_client(client_config_t config);
int             connect_to_server(client_config_t config);
void            receive_server_response(int sock);
void            send_message(int sock, const char *message);

#endif
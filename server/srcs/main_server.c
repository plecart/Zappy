#include "../includes/server.h"

int main(int argc, char *argv[]) {
    server_config_t config = parse_server_arguments(argc, argv);
    print_server_config(config);
    start_server(config);
    free(config.teams);
    return (0);
}
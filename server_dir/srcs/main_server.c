#include "../includes/server.h"

int main(int argc, char *argv[]) {
    server_config_t config = parse_server_arguments(argc, argv);
    signal(SIGPIPE, SIG_IGN);
    print_server_config(config);
    start_server(config);
    return (0);
}
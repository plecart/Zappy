#include "../includes/client.h"

int main(int argc, char *argv[]) {
    client_config_t config = parse_client_arguments(argc, argv);
    print_client_config(config);
    start_client(config);
    return 0;
}
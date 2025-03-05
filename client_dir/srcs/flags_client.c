#include "../includes/client.h"

void print_client_usage(char *prog_name)
{
    printf("Usage: %s -n <team> -p <port> [-h <hostname>]\n", prog_name);
    printf("-n team_name\n");
    printf("-p port\n");
    printf("-h name of the host, by default it'll be localhost\n");
    exit(EXIT_FAILURE);
}

client_config_t parse_client_arguments(int argc, char *argv[])
{
    client_config_t config = {0};
    config.hostname = "127.0.0.1";

    int opt;
    while ((opt = getopt(argc, argv, "n:p:h:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            config.team_name = optarg;
            break;
        case 'p':
            config.port = atoi(optarg);
            break;
        case 'h':
            config.hostname = optarg;
            break;
        default:
            print_client_usage(argv[0]);
            break;
        }
    }

    if (!config.team_name || config.port == 0)
    {
        print_client_usage(argv[0]);
    }

    return config;
}

void print_client_config(client_config_t config)
{
    printf("- - - - - - - - - - - - - - - - - - -\n");
    printf("Configuration du client :\n");
    printf("Nom de l'équipe: %s\n", config.team_name);
    printf("Nom d'hôte: %s\n", config.hostname);
    printf("Port: %d\n", config.port);
    printf("- - - - - - - - - - - - - - - - - - -\n");
}

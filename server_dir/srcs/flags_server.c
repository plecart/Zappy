#include "../includes/server.h"

void print_server_usage(char *prog_name)
{
    printf("Usage: %s -p <port> -x <width> -y <height> -n <team1> [<team2> ...] -c <nb> -t <t>\n", prog_name);
    printf("-p port number\n");
    printf("-x world width\n");
    printf("-y world height\n");
    printf("-n team_name_1 team_name_2 ...\n");
    printf("-c number of clients authorized at the beginning of the game\n");
    printf("-t time unit divider (the greater t is, the faster the game will go)\n");
    exit(EXIT_FAILURE);
}

server_config_t parse_server_arguments(int argc, char *argv[])
{
    if (argc < 7)
    {
        print_server_usage(argv[0]);
    }

    server_config_t config = {0};
    config.teams = malloc(sizeof(char *) * argc);
    config.team_count = 0;

    int opt;
    while ((opt = getopt(argc, argv, "p:x:y:n:c:t:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            config.port = atoi(optarg);
            break;
        case 'x':
            config.width = atoi(optarg);
            break;
        case 'y':
            config.height = atoi(optarg);
            break;
        case 'n':
            optind--;
            while (optind < argc && argv[optind][0] != '-')
            {
                config.teams[config.team_count++] = argv[optind++];
            }
            break;
        case 'c':
            config.clients_per_team = atoi(optarg);
            break;
        case 't':
            config.time_unit = atoi(optarg);
            break;
        default:
            print_server_usage(argv[0]);
            break;
        }
    }

    if (config.port == 0 || config.width == 0 || config.height == 0 ||
        config.team_count == 0 || config.clients_per_team == 0 || config.time_unit == 0)
    {
        print_server_usage(argv[0]);
    }

    return config;
}

void print_server_config(server_config_t config)
{
    printf("- - - - - - - - - - - - - - - - - - -\n");
    printf("Configuration du serveur :\n");
    printf("Port: %d\n", config.port);
    printf("Taille du monde: %dx%d\n", config.width, config.height);
    printf("Nombre de clients par équipe: %d\n", config.clients_per_team);
    printf("Diviseur du temps: %d\n", config.time_unit);
    printf("Équipes (%d):\n", config.team_count);
    for (int i = 0; i < config.team_count; i++)
    {
        printf("%d - %s\n", i + 1, config.teams[i]);
    }
    printf("- - - - - - - - - - - - - - - - - - -\n");
}
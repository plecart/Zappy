#include "../includes/server.h"

// Initialise un serveur TCP et le met en écoute sur un port donné.
// Cette fonction crée un socket, configure son adresse, lie ce socket à une
// adresse spécifique, et le met en écoute afin d'accepter des connexions clients.

int init_server(int port)
{
    int server_socket;
    struct sockaddr_in server_addr;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la création du socket\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors du bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, SOMAXCONN) < 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors du listen\n");
        exit(EXIT_FAILURE);
    }

    log_printf(PRINT_INFORMATION, "Serveur démarré sur le port %d\n", port);
    return server_socket;
}

// Gère les messages des clients connectés et traite les déconnexions.
// Cette fonction parcourt la liste des clients actifs et vérifie si un message a été reçu
// à l'aide de `FD_ISSET`. Si un client a envoyé un message, celui-ci est lu, affiché, puis
// une réponse simple ("OK\n") est envoyée. Si un client se déconnecte, son socket est fermé
// et retiré de la liste des clients actifs.

void handle_client_messages(player_t *players[], int max_players, fd_set *read_fds)
{
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < max_players; i++)
    {
        if (players[i] != NULL && FD_ISSET(players[i]->socket, read_fds))
        {

            int bytes_read = read(players[i]->socket, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0)
            {
                log_printf(PRINT_INFORMATION, "Client déconnecté (socket %d)\n", players[i]->socket);
                close(players[i]->socket);
                free(players[i]);
                players[i] = NULL;
            }
            else
            {
                buffer[bytes_read] = '\0';

                char *command = strtok(buffer, "\n");
                while (command != NULL)
                {
                    add_action_to_player(players[i], command);
                    command = strtok(NULL, "\n");
                }
            }
        }
    }
}

void send_message_player(player_t player, const char *message)
{
    server_send_message(player.socket, message, player.team_name);
}

void send_message_egg(egg_t egg, const char *message)
{
    server_send_message(egg.mother_socket, message, egg.team_name);
}

void server_send_message(int socket, const char *message, char *team_name)
{
    if (write(socket, message, strlen(message)) < 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors de l'envoi du message\n");
    }
    else
    {
        log_printf(PRINT_SEND, "[serveur], a [%d][%s]: %s", socket, team_name, message);
    }
}

// Démarre et gère le serveur en acceptant et en traitant les connexions clients.
// Cette fonction initialise un serveur, gère les connexions entrantes et traite les
// messages des clients en utilisant `select` pour surveiller l'activité sur les sockets.

void start_server(server_config_t config)
{
    int server_socket = init_server(config.port);
    int max_clients = 8 * config.team_count + 1;
    player_t *players[max_clients];
    memset(players, 0, sizeof(players));
    fd_set read_fds;
    int max_fd, activity;
    map_t *map = create_map(config.width, config.height);
    struct timeval timeout;
    egg_t *eggs[config.team_count * 8];
    memset(eggs, 0, sizeof(eggs));
    int egg_count = 0;
    int graphic_socket = -1; // Stockage du socket graphique
    bool game_started = false;

    populate_map(map);

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        max_fd = server_socket;

        if (graphic_socket != -1)
        {
            FD_SET(graphic_socket, &read_fds);
            if (graphic_socket > max_fd)
                max_fd = graphic_socket;
        }

        int i = 0;
        while (players[i] != NULL && players[i]->socket > 0)
        {
            FD_SET(players[i]->socket, &read_fds);
            if (players[i]->socket > max_fd)
                max_fd = players[i]->socket;
            i++;
        }

        timeout.tv_sec = 0;
        timeout.tv_usec = (1000000 / config.time_unit);

        activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0)
        {
            log_printf(PRINT_ERROR, "Erreur lors de select\n");
            exit(EXIT_FAILURE);
        }

        if (game_started)
            add_egg_cycle(graphic_socket, eggs, egg_count);

        if (FD_ISSET(server_socket, &read_fds))
        {
            accept_new_client(server_socket, players, eggs, &egg_count, max_clients - 1, &config, map, &graphic_socket, &game_started);
        }

        handle_client_messages(players, max_clients - 1, &read_fds);

        if (game_started)
        { // La partie ne commence pas tant que le client graphique n'est pas là

            for (int i = 0; i < max_clients - 1; i++)
            {
                if (players[i] != NULL)
                {
                    bool is_alive = player_eat(graphic_socket, players[i]);
                    if (!is_alive)
                    {
                        free_player(players[i]);
                        for (int j = i; j < max_clients - 1; j++)
                            players[j] = players[j + 1];
                        if (are_players_dead(players, max_clients - 1))
                        {
                            log_printf(PRINT_INFORMATION, "Tous les joueurs sont morts, il m'y a pas de gagnant\n");
                            send_graphic_game_end(graphic_socket, "");
                            free_all(players, map, server_socket);
                        }
                    }
                    else
                    {
                        if (execute_player_action(graphic_socket, players[i], map, players, max_clients, eggs, &egg_count) == true)
                        {
                            log_printf(PRINT_INFORMATION, "L'équipe %s a gagné\n", players[i]->team_name);
                            send_graphic_game_end(graphic_socket, players[i]->team_name);
                            free_all(players, map, server_socket);
                        }
                    }
                }
            }
        }
    }
}

void free_all(player_t *players[], map_t *map, int server_socket){
    free_players(players);
    free_map(map);
    close(server_socket);
    exit(EXIT_SUCCESS);
}

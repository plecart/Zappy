#include "../includes/server.h"

// Initialise un serveur TCP et le met en écoute sur un port donné.
// Cette fonction crée un socket, configure son adresse, lie ce socket à une
// adresse spécifique, et le met en écoute afin d'accepter des connexions clients.

int init_server(int port) {
    int server_socket;
    struct sockaddr_in server_addr;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        log_printf(PRINT_ERROR, "Erreur lors de la création du socket\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_printf(PRINT_ERROR, "Erreur lors du bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, SOMAXCONN) < 0) {
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

void handle_client_messages(player_t *players[], int max_players, fd_set *read_fds) {
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < max_players; i++) {
        if (players[i] != NULL && FD_ISSET(players[i]->socket, read_fds)) {
            
            int bytes_read = read(players[i]->socket, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0) {
                log_printf(PRINT_INFORMATION, "Client déconnecté (socket %d)\n", players[i]->socket);
                close(players[i]->socket);
                free(players[i]);
                players[i] = NULL;
            } else {
                buffer[bytes_read] = '\0';

                char *command = strtok(buffer, "\n");
                while (command != NULL) {
                    add_action_to_player(players[i], command);
                    command = strtok(NULL, "\n");
                }
            }
        }
    }
}

// Démarre et gère le serveur en acceptant et en traitant les connexions clients.
// Cette fonction initialise un serveur, gère les connexions entrantes et traite les
// messages des clients en utilisant `select` pour surveiller l'activité sur les sockets.

void start_server(server_config_t config) {
    int server_socket = init_server(config.port);
    int max_clients = config.clients_per_team * config.team_count;
    player_t *players[max_clients];
    memset(players, 0, sizeof(players));
    fd_set read_fds;
    int max_fd, activity;
    map_t *map = create_map(config.width, config.height);
    struct timeval timeout;

    populate_map(map);

    while (1) {
        time_t start = time(NULL); // Timestamp pour la gestion du temps

        // Réinitialisation du set de lecture
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        max_fd = server_socket;

        // Ajout des sockets des joueurs actifs
        for (int i = 0; i < max_clients; i++) {
            if (players[i] && players[i]->socket > 0) {
                FD_SET(players[i]->socket, &read_fds);
                if (players[i]->socket > max_fd) max_fd = players[i]->socket;
            }
        }

        // Configuration du timeout : vérification toutes les 10ms
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;

        activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0) {
            log_printf(PRINT_ERROR, "Erreur lors de select\n");
            exit(EXIT_FAILURE);
        }

        // Vérification des nouvelles connexions
        if (FD_ISSET(server_socket, &read_fds)) {
            accept_new_client(server_socket, players, max_clients, &config);
        }

        // Gestion des messages reçus des clients
        handle_client_messages(players, max_clients, &read_fds);
       
        // Exécution des actiones des joueurs en attente
        for (int i = 0; i < max_clients; i++) {
            if (players[i]) {
                execute_player_action(players[i], map, players, max_clients);
            }
        }


        // Gestion du temps pour maintenir un cycle de 1 seconde
        time_t end = time(NULL);
        int elapsed_time = (int)(end - start);
        if (elapsed_time < 1) {
            struct timeval remaining_time = {0, (1 - elapsed_time) * 1000000};
            select(0, NULL, NULL, NULL, &remaining_time);
        }

        //print_players(players, max_clients);
        //log_printf(PRINT_INFORMATION, "- - - Cycle de jeu terminé - - -\n");
    }

    // Nettoyage des ressources
    free_players(players, max_clients);
    free_map(map);
    close(server_socket);
}


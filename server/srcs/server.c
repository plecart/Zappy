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

// Accepte une nouvelle connexion client et l'ajoute à la liste des clients actifs.
// Cette fonction attend une connexion entrante sur le socket serveur, accepte la connexion
// et enregistre le socket du client dans un tableau de gestion des connexions.

void accept_new_client(int server_socket, int *client_sockets, int max_clients) {
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);

    if (client_socket < 0) {
        log_printf(PRINT_ERROR, "Erreur lors de accept\n");
        return;
    }

    log_printf(PRINT_INFORMATION, "Nouvelle connexion acceptée (socket %d)\n", client_socket);

    for (int i = 0; i < max_clients; i++) {
        if (client_sockets[i] == 0) {
            client_sockets[i] = client_socket;
            log_printf(PRINT_INFORMATION, "Client ajouté à l'index %d\n", i);
            return;
        }
    }

    log_printf(PRINT_ERROR, "Connexion refusée : trop de clients\n");
    close(client_socket);
}

// Gère les messages des clients connectés et traite les déconnexions.
// Cette fonction parcourt la liste des clients actifs et vérifie si un message a été reçu
// à l'aide de `FD_ISSET`. Si un client a envoyé un message, celui-ci est lu, affiché, puis
// une réponse simple ("OK\n") est envoyée. Si un client se déconnecte, son socket est fermé
// et retiré de la liste des clients actifs.

void handle_client_messages(int *client_sockets, int max_clients, fd_set *read_fds) {
    
    for (int i = 0; i < max_clients; i++) {
        int sd = client_sockets[i];

        if (FD_ISSET(sd, read_fds)) {
            char buffer[BUFFER_SIZE] = {0};
            int bytes_read = read(sd, buffer, sizeof(buffer) - 1);

            if (bytes_read <= 0) {
                log_printf(PRINT_INFORMATION, "Client déconnecté (socket %d)\n", sd);
                close(sd);
                client_sockets[i] = 0;

            } else {
                buffer[bytes_read] = '\0';
                log_printf(PRINT_RECEIVE, "Message reçu (socket %d): %s", sd, buffer);
                send_message(sd, "OK\n");
                
            }
        }
    }
}

// Démarre et gère le serveur en acceptant et en traitant les connexions clients.
// Cette fonction initialise un serveur, gère les connexions entrantes et traite les
// messages des clients en utilisant `select` pour surveiller l'activité sur les sockets.

void start_server(server_config_t config) {

    int server_socket = init_server(config.port);

    map_t *map = create_map(config.width, config.height);
    populate_map(map);
    // print_map(map);

    int max_clients = (config.clients_per_team * config.team_count) + 1;
    int *client_sockets = calloc(max_clients, sizeof(int));
    fd_set read_fds;
    int max_fd, activity;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        max_fd = server_socket;

        for (int i = 0; i < max_clients; i++) {
            int sd = client_sockets[i];
            if (sd > 0) FD_SET(sd, &read_fds);
            if (sd > max_fd) max_fd = sd;
        }

        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0) {
            log_printf(PRINT_ERROR, "Erreur lors de select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(server_socket, &read_fds)) {
            accept_new_client(server_socket, client_sockets, max_clients);
        }
        
        handle_client_messages(client_sockets, max_clients, &read_fds);
    }

    free_map(map);
    free(client_sockets);
    close(server_socket);
}
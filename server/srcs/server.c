#include "../includes/server.h"

game_time_t game_time = {0, 100, false};  // Temps initialisé avec `t = 100`
int         *global_client_sockets;
int         global_max_clients;
player_t    *players = NULL;
int         player_count = 0;

// Initialise un serveur TCP et le met en écoute sur un port donné.
// Cette fonction crée un socket, configure son adresse, lie ce socket à une
// adresse spécifique, et le met en écoute afin d'accepter des connexions clients.

int init_server(int port) {
    int server_socket;
    struct sockaddr_in server_addr;

    log_printf("Démarrage du serveur sur le port %d...\n", port);
    
    // Création d'un socket IPv4 en mode TCP.
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        log_printf("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur (IPv4, écoute sur toutes les interfaces).
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Liaison du socket à l'adresse et au port spécifiés.
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_printf("Erreur lors du bind");
        exit(EXIT_FAILURE);
    }

    // Mise en écoute du socket pour accepter les connexions entrantes.
    if (listen(server_socket, SOMAXCONN) < 0) {
        log_printf("Erreur lors du listen");
        exit(EXIT_FAILURE);
    }

    // Affichage d'un message indiquant que le serveur est démarré.
    log_printf("Serveur démarré sur le port %d\n", port);
    return server_socket;
}

// Accepte une nouvelle connexion client et l'ajoute à la liste des clients actifs.
// Cette fonction attend une connexion entrante sur le socket serveur, accepte la connexion
// et enregistre le socket du client dans un tableau de gestion des connexions.

void accept_new_client(int server_socket, int *client_sockets, int max_clients, map_t *map) {
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    // Attente d'une connexion entrante avec `accept`.
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);

    // Vérification du succès de l'acceptation.
    if (client_socket < 0) {
        log_printf("Erreur lors de accept");
        return;
    }

    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, sizeof(buffer) - 1);

    if (strcmp(buffer, "GRAPHIC\n") == 0)
    {
        log_printf("Client graphique connecté ! Démarrage du jeu...\n");
        game_time.game_started = true;
        send_message_to_all_clients("La partie commence !\n", client_sockets, max_clients);
    }
    else
    {
    // Affichage d'un message confirmant la connexion d'un nouveau client.
    log_printf("Nouvelle connexion acceptée (socket %d)\n", client_socket);

    // Recherche d'un emplacement libre dans le tableau `client_sockets` pour stocker le
    // descripteur du nouveau client.
    for (int i = 0; i < max_clients; i++) {
        if (client_sockets[i] == 0) {
            client_sockets[i] = client_socket;
            log_printf("Client ajouté à l'index %d\n", i);
            
            
            players = realloc(players, (player_count + 1) * sizeof(player_t));
            players[player_count].id = player_count;
            players[player_count].socket = client_socket;
            players[player_count].team_name = "TEAM NAME";
            assign_player_position(&players[player_count], map);
            player_count++;

            log_printf("Joueur #%d placé en (%d, %d)\n",
                players[player_count - 1].id,
                players[player_count - 1].x,
                players[player_count - 1].y
            );

            // Envoyer la position au client
            char response[64];
            snprintf(response, sizeof(response), "%d\n%d %d", max_clients - player_count, players[player_count - 1].x, players[player_count - 1].y);
            write(client_socket, response, strlen(response));
            return;
        }
    }

    //  Si aucun emplacement n'est disponible, la connexion est refusée et le socket est fermé.
    log_printf("Connexion refusée : trop de clients\n");
    close(client_socket);
    }
}

// Gère les messages des clients connectés et traite les déconnexions.
// Cette fonction parcourt la liste des clients actifs et vérifie si un message a été reçu
// à l'aide de `FD_ISSET`. Si un client a envoyé un message, celui-ci est lu, affiché, puis
// une réponse simple ("OK\n") est envoyée. Si un client se déconnecte, son socket est fermé
// et retiré de la liste des clients actifs.

void handle_client_messages(int *client_sockets, int max_clients, fd_set *read_fds) {
    
    // Parcours de la liste des sockets clients.
    for (int i = 0; i < max_clients; i++) {
        int sd = client_sockets[i];

        // Vérification si un client a envoyé un message en utilisant `FD_ISSET`.
        if (FD_ISSET(sd, read_fds)) {
            char buffer[BUFFER_SIZE] = {0};
            int bytes_read = read(sd, buffer, sizeof(buffer) - 1);

            // Si `read` retourne 0 ou une valeur négative, le client est considéré comme déconnecté
            if (bytes_read <= 0) {
                log_printf("Client déconnecté (socket %d)\n", sd);
                close(sd);
                client_sockets[i] = 0;

            // Sinon une réponse "OK\n" est envoyée au client via `write`
            } else {
                buffer[bytes_read] = '\0';
                log_printf("Message reçu (socket %d): %s\n", sd, buffer);
                enqueue_command(&players[i], buffer);
            }
        }
    }
}

void send_message_to_all_clients(const char *message, int *client_sockets, int max_clients) {
    for (int i = 0; i < max_clients; i++) {
        if (client_sockets[i] != 0) {
            write(client_sockets[i], message, strlen(message));
        }
    }
}

// Démarre et gère le serveur en acceptant et en traitant les connexions clients.
// Cette fonction initialise un serveur, gère les connexions entrantes et traite les
// messages des clients en utilisant `select` pour surveiller l'activité sur les sockets.

void start_server(server_config_t config) {

    // Initialisation du serveur avec `init_server`, retournant un socket d'écoute.
    int server_socket = init_server(config.port);

    // Création de la carte
    map_t *map = create_map(config.width, config.height);
    populate_map(map);
    
    // Affichage initial de la carte
    // log_printf("Carte générée :\n");
    // print_map(map);

    // Calcul du nombre maximal de clients (`max_clients`) et allocation dynamique d'un tableau
    // pour stocker les descripteurs des clients connectés.
    int max_clients = (config.clients_per_team * config.team_count) + 1;
    int *client_sockets = calloc(max_clients, sizeof(int));

    global_client_sockets = client_sockets;
    global_max_clients = max_clients;
    game_time.time_unit = config.time_unit;

    fd_set read_fds;
    int max_fd, activity;

    pthread_t time_thread;
    pthread_create(&time_thread, NULL, game_loop, (void *)map);

    // Boucle infinie de gestion des connexions et des messages
    while (1) {
        //  Réinitialisation et configuration de `read_fds` pour surveiller l'activité des sockets.
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        max_fd = server_socket;

        //  Définition du plus grand descripteur de fichier pour `select`.
        for (int i = 0; i < max_clients; i++) {
            int sd = client_sockets[i];
            if (sd > 0) FD_SET(sd, &read_fds);
            if (sd > max_fd) max_fd = sd;
        }

        // Utilisation de `select` pour détecter les connexions ou messages entrants.
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            log_printf("Erreur lors de select");
            exit(EXIT_FAILURE);
        }

        // Si une connexion est détectée sur le socket serveur, elle est acceptée avec `accept_new_client`.
        if (FD_ISSET(server_socket, &read_fds)) {
            accept_new_client(server_socket, client_sockets, max_clients, map);
        }

        // Les messages des clients sont ensuite traités avec `handle_client_messages`.
        handle_client_messages(client_sockets, max_clients, &read_fds);
    }

    free_map(map);

    // Libération de la mémoire allouée pour `client_sockets` et fermeture du socket serveur
    free(client_sockets);
    close(server_socket);
}

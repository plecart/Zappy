#include "../includes/client.h"

// Établit une connexion TCP avec un serveur spécifié dans la configuration client.
// Cette fonction crée un socket, configure l'adresse du serveur, et tente de se connecter.
// En cas d'échec à n'importe quelle étape, elle affiche un message d'erreur et retourne -1.
// En cas de succès, elle retourne le descripteur du socket connecté.

int connect_to_server(client_config_t config)
{
    int sock;
    struct sockaddr_in server_addr;

    // Création d'un socket TCP avec `socket()`.
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la création du socket\n");
        return -1;
    }

    // Définition du domaine (`AF_INET`), du port (converti avec `htons()`), et de l'adresse IP.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config.port);

    // Conversion de l'adresse IP avec `inet_pton()`, avec vérification de sa validité.
    if (inet_pton(AF_INET, config.hostname, &server_addr.sin_addr) <= 0)
    {
        log_printf(PRINT_ERROR, "Adresse du serveur invalide\n");
        close(sock);
        return -1;
    }

    // Tentative de connexion au serveur avec `connect()`
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        log_printf(PRINT_ERROR, "Connexion au serveur échouée\n");
        close(sock);
        return -1;
    }

    log_printf(PRINT_INFORMATION, "Connecté au serveur %s:%d\n", config.hostname, config.port);
    return sock;
}

// Reçoit et affiche la réponse du serveur via un socket.
// Cette fonction attend un message du serveur, le lit dans un tampon et l'affiche.
// En cas d'erreur lors de la lecture, un message d'erreur est affiché.

int receive_server_response(int sock, char RESPONSES_TAB, int response_count)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    int bytes_read = read(sock, buffer, BUFFER_SIZE - 1);
    if (bytes_read <= 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la réception de la réponse\n");
        return 0;
    }

    buffer[bytes_read] = '\0';
    
    char *response = strtok(buffer, "\n");
    int response_index = response_count;

    while (response != NULL && response_index < MAX_RESPONSES_COMMANDS)
    {
        strncpy(responses[response_index], response, BUFFER_SIZE - 1);
        responses[response_index][BUFFER_SIZE - 1] = '\0'; // Ensure null termination
        response_index++;
        response = strtok(NULL, "\n");
    }
    return response_index;
}

// Initialise et exécute un client pour se connecter au serveur.
// Cette fonction établit une connexion avec le serveur, envoie le nom d'équipe,
// reçoit une réponse du serveur, puis ferme la connexion.

void start_client(client_config_t config, bool is_slave)
{
    int sock = connect_to_server(config);
    if (sock == -1)
        return;

    char RESPONSES_TAB;
    send_message(sock, strcat(config.team_name, "\n"));
    sleep(1);
    receive_server_response(sock, responses, 0);

    !is_slave ? brain(sock, config) : slave(sock);


    // Fermeture du socket avec `close(sock)`, suivie d'un message indiquant la fin de la connexion.
    close(sock);
    log_printf(PRINT_INFORMATION, "Connexion fermée.\n");
}

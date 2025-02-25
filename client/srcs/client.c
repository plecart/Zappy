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
    char *total = malloc(BUFFER_SIZE);
    if (total == NULL)
    {
        log_printf(PRINT_ERROR, "Erreur lors de l'allocation du tampon de réception\n");
        exit(0);
    }
    memset(total, 0, BUFFER_SIZE);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int total_len = 0;
    while (1)
    {
        // printf("debut de read boucle\n");
        int bytes_read = read(sock, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0)
        {
            log_printf(PRINT_ERROR, "Erreur lors de la réception de la réponse\n");
            exit(0);
        }
        if (bytes_read == 0) // Fin de fichier (aucune donnée à lire)
        {
            break; // Quitter la boucle
        }
        buffer[bytes_read] = '\0';
        int new_len = total_len + bytes_read;
        total = realloc(total, new_len + 1); // +1 pour '\0'
        if (total == NULL)
        {
            log_printf(PRINT_ERROR, "Erreur de réallocation de mémoire pour total\n");
            exit(0);
        }
        strncat(total, buffer, bytes_read); // Ajoute buffer à la fin de total
        total_len = new_len;
        // printf("BBB %d != %d\n", bytes_read, BUFFER_SIZE - 1);
        if (bytes_read != BUFFER_SIZE - 1 || buffer[bytes_read - 1] == '\n')
        {
            break; // Fin de la réponse
        }
        // printf("[%s]\n", buffer);
    }

    // printf("???\n");
    // total[total_len + 1] = '\0';
    // printf("TOTAL = [%s]\n", total);
    char *response = strtok(total, "\n");
    // printf("response: %s\n", response);
    int response_index = response_count;

    while (response != NULL && response_index < MAX_RESPONSES_COMMANDS)
    {
        int response_len = strlen(response);
        responses[response_index] = malloc(response_len + 1);
        if (responses[response_index] == NULL)
        {
            log_printf(PRINT_ERROR, "Erreur lors de l'allocation de la réponse\n");
            exit(0);
        }
        strncpy(responses[response_index], response, response_len);
        responses[response_index][response_len] = '\0'; // Ensure null termination
        response_index++;
        response = strtok(NULL, "\n");
    }
    free(total);
    total = NULL;
    return response_index;
}

// Initialise et exécute un client pour se connecter au serveur.
// Cette fonction établit une connexion avec le serveur, envoie le nom d'équipe,
// reçoit une réponse du serveur, puis ferme la connexion.

void start_client(client_config_t config, bool is_slave)
{
    static int client_number = -1;

    ++client_number;
    int sock = connect_to_server(config);
    if (sock == -1)
        return;
    sleep(2);
    send_message(sock, strcat(config.team_name, "\n"));
    char RESPONSES_TAB;
    memset(responses, 0, sizeof(responses));

    int response_count = receive_server_response(sock, responses, 0);

    printf("-1---------\n");
    print_responses(responses, response_count);
    printf("-2---------\n");

    filter_responses(responses, &response_count, config, is_slave);
    print_responses(responses, response_count);

    if (is_slave)
    {
        bool found_mission = false;
        while (!found_mission)
        {
           // printf("FOUND MISSION\n");
            response_count = receive_server_response(sock, responses, response_count);
            //printf("1 --->\n");
           // print_responses(responses, response_count);
            filter_responses(responses, &response_count, config, is_slave);
           // printf("2 --->\n");
           // print_responses(responses, response_count);
            if (response_count != 0 && strstr(responses[0], "mission") != NULL)
                found_mission = true;
           // printf("3 --->\n");
          //  print_responses(responses, response_count);
        }
    }

    // printf("apres filtre\n");
    // print_responses(responses, response_count);
    // print_responses(responses, response_count);

    printf("-3----------\n");

    !is_slave ? brain(responses, response_count, sock, config) : slave(responses, response_count, sock, config);
    // Fermeture du socket avec `close(sock)`, suivie d'un message indiquant la fin de la connexion.
    close(sock);
    log_printf(PRINT_INFORMATION, "Connexion fermée.\n");
}

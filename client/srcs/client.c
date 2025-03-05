#include "../includes/client.h"

int connect_to_server(client_config_t config)
{
    int sock;
    struct sockaddr_in server_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la création du socket\n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config.port);

    if (inet_pton(AF_INET, config.hostname, &server_addr.sin_addr) <= 0)
    {
        log_printf(PRINT_ERROR, "Adresse du serveur invalide\n");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        log_printf(PRINT_ERROR, "Connexion au serveur échouée\n");
        close(sock);
        return -1;
    }

    log_printf(PRINT_INFORMATION, "Connecté au serveur %s:%d\n", config.hostname, config.port);
    return sock;
}

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
        int bytes_read = read(sock, buffer, BUFFER_SIZE - 1);
        if (bytes_read < 0)
        {
            log_printf(PRINT_ERROR, "Erreur lors de la réception de la réponse\n");
        }
        if (bytes_read == 0)
        {
            break; 
        }
        if (bytes_read > 0) 
        {
            buffer[bytes_read] = '\0';
            int new_len = total_len + bytes_read;
            total = realloc(total, new_len + 1); 
            if (total == NULL)
            {
                log_printf(PRINT_ERROR, "Erreur de réallocation de mémoire pour total\n");
                exit(0);
            }
            strncat(total, buffer, bytes_read);
            total_len = new_len;
            if (bytes_read != BUFFER_SIZE - 1 || buffer[bytes_read - 1] == '\n')
            {
                break; 
            }

        }
    }

    char *response = strtok(total, "\n");
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
        responses[response_index][response_len] = '\0';
        response_index++;
        response = strtok(NULL, "\n");
    }
    free(total);
    total = NULL;
    return response_index;
}

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

    if (is_slave)
    {
        bool found_mission = false;
        while (!found_mission)
        {
            response_count = receive_server_response(sock, responses, response_count);
            filter_responses(responses, &response_count, config, is_slave);
            if (response_count != 0 && strstr(responses[0], "mission") != NULL)
                found_mission = true;
        }
    }
    !is_slave ? brain(responses, response_count, sock, config) : slave(responses, response_count, sock, config);
    free_all_responses(responses, &response_count);
    close(sock);
    log_printf(PRINT_INFORMATION, "Connexion fermée.\n");
}

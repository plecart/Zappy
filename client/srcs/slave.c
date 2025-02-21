#include "../includes/client.h"

void start_slave(client_config_t config)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        start_client(config, true);
        exit(0);
    }
    else if (pid < 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la création du processus fils\n");
    }
}

void slave(int sock)
{
    char RESPONSES_TAB;
    int response_count = 0;

    while (1)
    {
        execute_action(sock, "droite\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        execute_action(sock, "droite\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        scan_for_resource(sock, responses, &response_count, NOURRITURE);
        print_responses(responses, response_count);
    }
}
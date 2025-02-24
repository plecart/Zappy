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

void slave(char RESPONSES_TAB, int response_count, int sock, client_config_t config)
{
    char mission[BUFFER_SIZE_TINY];
    int quantity_needed = 0;

    printf("NEW SLAVE : [%s][%s]\n", config.team_name, responses[0]);
    if ((quantity_needed = get_mission(responses[0], config.team_name, mission)) == -1)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la récupération de la mission\n");
        return;
    }

    int quantity = 0;

    while (quantity < quantity_needed)
    {
        printf("QUANTITY: %d\n", quantity);
        scan_for_resource(sock, responses, &response_count, mission);
        quantity = inventory(sock, responses, &response_count, mission);
    }

    printf("DONE[%d][%s]\n", quantity, mission);
    while (1)
    {
        // execute_action(sock, "droite\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        // execute_action(sock, "droite\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        // scan_for_resource(sock, responses, &response_count, NOURRITURE);
        // printf("RESPONSE: %d\n", response_count);
        // print_responses(responses, response_count);
    }
}
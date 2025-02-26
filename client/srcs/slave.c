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

    char trim_team_name[BUFFER_SIZE_TINY];
    strcpy(trim_team_name, config.team_name);
    trim_team_name[strlen(trim_team_name) - 2] = '\0';

    // printf("NEW SLAVE : [%s][%s]\n", config.team_name, responses[0]);
    if ((quantity_needed = get_mission(responses[0], trim_team_name, mission)) == -1)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la récupération de la mission\n");
        return;
    }

    int quantity = 0;

    printf("DEBUT DE RECHERCHE [%s][%d]\n", mission, quantity_needed);
    while (quantity < quantity_needed)
    {
        printf("BOUCLE CHERCHER\n");
        if (4 > inventory(sock, responses, &response_count, NOURRITURE))
            scan_for_resource(sock, responses, &response_count, NOURRITURE);

        printf("quantity = %d (%s) < %d\n", quantity, mission, quantity_needed);
        scan_for_resource(sock, responses, &response_count, mission);
        quantity = inventory(sock, responses, &response_count, mission);
    }
    printf("BOucle de recherche terminée\n");

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "broadcast %s done\n", trim_team_name);
    execute_action(sock, buffer, responses, &response_count, SERVER_RESPONSE_OK_KO, true);

    while(1)
    {}
    // while (1)
    // {
    //     printf("MISSION DONE\n");
    //     int response_index = -1;
    //     while (response_index == -1)
    //     {
    //         response_count = receive_server_response(sock, responses, response_count);
    //         printf("AVANT FILTRE ---\n");
    //         print_responses(responses, response_count);
    //         filter_responses(responses, &response_count, config, true);
    //         printf("APRES FILTRE ---\n");
    //         print_responses(responses, response_count);
    //         printf("MESSAGE DU SERV : %s\n", responses[response_index]);
    //         response_index = get_response_index(responses, SERVER_RESPONSE_BEACON, response_count);
    //         printf("RESPONS COUNT = %d | response_index = %d\n", response_count, response_index);

    //         delete_response(responses, &response_count, response_index);
    //     }

    //     //   print_responses(responses, response_count);
    // }
}
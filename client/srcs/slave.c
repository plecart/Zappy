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

    printf("NEW SLAVE : [%s][%s]\n", config.team_name, responses[0]);
    if ((quantity_needed = get_mission(responses[0], trim_team_name, mission)) == -1)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la récupération de la mission\n");
        return;
    }

    int quantity = 0;

    printf("slave - DEBUT DE RECHERCHE [%s][%d]\n", mission, quantity_needed);
    while (quantity < quantity_needed)
    {
        printf("slave - BOUCLE CHERCHER\n");
        while (30 > inventory(sock, responses, &response_count, NOURRITURE))
            scan_for_resource(sock, responses, &response_count, NOURRITURE);

        printf("slave - quantity = %d (%s) < %d\n", quantity, mission, quantity_needed);
        scan_for_resource(sock, responses, &response_count, mission);
        quantity = inventory(sock, responses, &response_count, mission);
    }
    printf("slave - BOucle de recherche terminée [%s]\n", mission);

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "broadcast %s done %s\n", trim_team_name, mission);
    execute_action(sock, buffer, responses, &response_count, SERVER_RESPONSE_OK_KO, true);

    printf("slave - MISSION DONE\n");
    int message_origin = -1;
    while (message_origin != 0)
    {
        int response_index = -1;
        while (response_index == -1)
        {
            while (response_count > 0)
            {
                printf("{%d}\n", response_count);
                delete_response(responses, &response_count, 0);
            }
            // printf(".slave - %s\n", mission);
            response_count = receive_server_response(sock, responses, response_count);
            // printf("AVANT FILTRE ---\n");
            // print_responses(responses, response_count);
            filter_responses(responses, &response_count, config, true);
            filter_slaves_extra_responses(responses, &response_count);
            // printf("APRES FILTRE ---\n");
            print_responses(responses, response_count);
            // printf("MESSAGE DU SERV : %s\n", responses[response_index]);
            response_index = get_response_index(responses, SERVER_RESPONSE_BEACON, response_count);
            // printf("RESPONS COUNT = %d | response_index = %d\n", response_count, response_index);
        }

        message_origin = get_message_direction(responses[response_index]);
        printf("slave - ORIGIN : %d - %s [%s]\n", message_origin, responses[response_index], mission);
        if (message_origin != 0)
            one_step_to_master(message_origin, sock, responses, &response_count);
    }

    printf("slave - ARRIVE %s\n", mission);

    printf("SLAVE POSE %s\n", mission);
    while (quantity_needed > 0)
    {
        sprintf(buffer, "pose %s\n", mission);
        execute_action(sock, buffer, responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        printf("%s poser[%d]\n", mission, quantity_needed);
        print_responses(responses, response_count);
        quantity_needed--;
    }
    
    // for (int i = 0; i < quantity_needed; i++)
    // {
    //     sprintf(buffer, "pose %s\n", mission);
    //     execute_action(sock, buffer, responses, &response_count, SERVER_RESPONSE_OK_KO, true);
    // }

    printf("FINI POSER %s\n", mission);
    while (1)
    {
    }
}

void one_step_to_master(int direction, int sock, char RESPONSES_TAB, int *response_count)
{
    char buffer[BUFFER_SIZE_TINY];

    if (direction == 8 || direction == 2)
    {
        sprintf(buffer, "avance\n");
        execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
        if (direction == 8)
            sprintf(buffer, "gauche\n");
        else if (direction == 2)
            sprintf(buffer, "droite\n");
        execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
    }
    else if (direction == 3 || direction == 4)
    {
        sprintf(buffer, "droite\n");
        execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
        if (direction == 4)
        {
            sprintf(buffer, "avance\n");
            execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
            sprintf(buffer, "droite\n");
            execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
        }
    }
    else if (direction == 7 || direction == 6)
    {
        sprintf(buffer, "gauche\n");
        execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
        if (direction == 6)
        {
            sprintf(buffer, "avance\n");
            execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
            sprintf(buffer, "gauche\n");
            execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
        }
    }
    else if (direction == 5)
    {
        sprintf(buffer, "droite\n");
        execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
        sprintf(buffer, "droite\n");
        execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
    }
    sprintf(buffer, "avance\n");
    execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
}

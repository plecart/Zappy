#include "../includes/client.h"

bool start_slave(client_config_t config)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        start_client(config, true);
        return false;
    }
    else if (pid < 0)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la création du processus fils\n");
    }
    return true;
}

void slave(char RESPONSES_TAB, int response_count, int sock, client_config_t config)
{
    char mission[BUFFER_SIZE_TINY];
    int quantity_needed = 0;

    char trim_team_name[BUFFER_SIZE_TINY];
    strcpy(trim_team_name, config.team_name);
    trim_team_name[strlen(trim_team_name) - 2] = '\0';

    log_printf(PRINT_INFORMATION, "[Slave][0] - get mission from (%s)\n", responses[0]);
    if ((quantity_needed = get_mission(responses[0], trim_team_name, mission)) == -1)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la récupération de la mission\n");
        return;
    }

    int quantity = 0;

    log_printf(PRINT_INFORMATION, "[Slave][1] (%s) - se rempli de nourriture et cherche sa pierre\n", mission);
    while (quantity < quantity_needed)
    {
        while (25 > inventory(sock, responses, &response_count, NOURRITURE))
            scan_for_resource(sock, responses, &response_count, NOURRITURE);
        scan_for_resource(sock, responses, &response_count, mission);
        quantity = inventory(sock, responses, &response_count, mission);
    }

    char buffer[BUFFER_SIZE];
    log_printf(PRINT_INFORMATION, "[Slave][2] (%s) - signal qu'il a fini\n", mission);
    sprintf(buffer, "broadcast %s done %s\n", trim_team_name, mission);
    execute_action(sock, buffer, responses, &response_count, SERVER_RESPONSE_OK_KO, true);
    int message_origin = -1;
    log_printf(PRINT_INFORMATION, "[Slave][3] (%s) - suite le signal du \"master\"\n", mission);
    while (message_origin != 0)
    {
        int response_index = -1;
        while (response_index == -1)
        {
            while (response_count > 0)
            {
                delete_response(responses, &response_count, 0);
            }
            response_count = receive_server_response(sock, responses, response_count);
            filter_responses(responses, &response_count, config, true);
            filter_slaves_extra_responses(responses, &response_count);
            response_index = get_response_index(responses, SERVER_RESPONSE_BEACON, response_count);
        }
        message_origin = get_message_direction(responses[response_index]);
        if (message_origin != 0)
            one_step_to_master(message_origin, sock, responses, &response_count);
    }

    log_printf(PRINT_INFORMATION, "[Slave][4] (%s) - pose toutes ses ressources (%d)\n", mission, quantity_needed);
    while (quantity_needed > 0)
    {
        sprintf(buffer, "pose %s\n", mission);
        execute_action(sock, buffer, responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        printf("%s poser[%d]\n", mission, quantity_needed);
        quantity_needed--;
    }

    log_printf(PRINT_INFORMATION, "[Slave][5] (%s) - Attend la fin de la partie\n", mission, quantity_needed);
    bool over = false;

    while (over == false)
    {
        response_count = receive_server_response(sock, responses, response_count);
        if (get_response_index(responses, SERVER_RESPONSE_OVER, response_count) != -1)
            over = true;
    }

    free_all_responses(responses, &response_count);
    log_printf(PRINT_INFORMATION, "[Slave][6] (%s) - Quitte\n", mission);
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

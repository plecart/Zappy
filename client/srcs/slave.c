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

    //printf("NEW SLAVE : [%s][%s]\n", config.team_name, responses[0]);
    if ((quantity_needed = get_mission(responses[0], trim_team_name, mission)) == -1)
    {
        log_printf(PRINT_ERROR, "Erreur lors de la récupération de la mission\n");
        return;
    }

    int quantity = 0;

    while (quantity < quantity_needed)
    {
        printf("quantity = %d (%s) < %d\n", quantity,mission, quantity_needed);
        scan_for_resource(sock, responses, &response_count, mission);
        quantity = inventory(sock, responses, &response_count, mission);
    }

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "broadcast %s done\n", trim_team_name);
    execute_action(sock, buffer, responses, &response_count, SERVER_RESPONSE_OK_KO, true);
    
    while (1)
    {
     //   print_responses(responses, response_count);
     //   filter_responses (responses, &response_count, config, true);
     //   print_responses(responses, response_count);
      //  printf("---\n");
      //  sleep(2);
        // execute_action(sock, "droite\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        // execute_action(sock, "droite\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        // scan_for_resource(sock, responses, &response_count, NOURRITURE);
        // printf("RESPONSE: %d\n", response_count);
        // print_responses(responses, response_count);
    }
}
#include "../includes/client.h"

int level_up_requirement[7][6] = {
    {1, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 0, 0},
    {2, 0, 1, 0, 2, 0},
    {1, 1, 2, 0, 1, 0},
    {1, 2, 1, 3, 0, 0},
    {1, 2, 3, 0, 1, 0},
    {2, 2, 2, 2, 2, 1}};

void brain(char RESPONSES_TAB, int response_count, int sock, client_config_t config)
{
    log_printf(PRINT_INFORMATION, "[Brain][1] - Recolte nourriture et pond des oeufs\n");
    for (int i = 0; i < 6; i++)
    {
        scan_for_resource(sock, responses, &response_count, NOURRITURE);
        // printf("???\n");
        execute_action(sock, "fork\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
    }

    log_printf(PRINT_INFORMATION, "[Brain][2] - Attend l'ecolsion, donne la mission, recolte nourriture\n");
    int slave_count = 0;
    while (slave_count < 6)
    {
        if (did_egg_hatched(responses, &response_count) == true)
        {
            printf("EGG\n");
            if (start_slave(config) == false)
            {
                free_all_responses(responses, &response_count);
                return;
            }
            sleep(1);
            broadcast_mission(sock, responses, &response_count, config.team_name);
            slave_count++;
        }
        if (7 > inventory(sock, responses, &response_count, NOURRITURE))
            scan_for_resource(sock, responses, &response_count, NOURRITURE);
    }

    log_printf(PRINT_INFORMATION, "[Brain][3] - Attend que les \"slaves\" remplisse leur mission\n");
    int slave_ready = 0;
    while (slave_ready != 6)
    {
        if (7 > inventory(sock, responses, &response_count, NOURRITURE))
            scan_for_resource(sock, responses, &response_count, NOURRITURE);
        print_responses(responses, response_count);
        filter_responses(responses, &response_count, config, false);
        int ret = 0;

        if ((ret = is_slave_ready(responses, &response_count)) != 0)
        {
            printf("Slave READY [%d]\n", ret);
            //print_responses(responses, response_count);
            slave_ready += ret;
        }
    }

    log_printf(PRINT_INFORMATION, "[Brain][4] - Stock 20 de nourriture\n");
    while (25 > inventory(sock, responses, &response_count, NOURRITURE))
        scan_for_resource(sock, responses, &response_count, NOURRITURE);

    log_printf(PRINT_INFORMATION, "[Brain][5] - Appel les \"slaves\" jusqu'a lui\n");
    int slave_arrived = 0;
    while (slave_arrived != 6)
    {
        slave_arrived = how_much_players(sock, responses, &response_count);
        printf("BEFORE BEACON %d\n", slave_arrived);
        broadcast_beacon(sock, responses, &response_count, config.team_name);
    }

    bool over = false;
    int level = 1;
    log_printf(PRINT_INFORMATION, "[Brain][6] - Fait l'incantation en boucle\n");
    while (level < 8 && over == false)
    {
        if (enough_resources(sock, responses, &response_count, level_up_requirement[level - 1]) == true)
        {
            char buffer[BUFFER_SIZE_TINY];
            sprintf(buffer, "incantation\n");
            execute_action(sock, buffer, responses, &response_count, SERVER_RESPONSE_INCANTATION, true);
            level++;
        }
        if (get_response_index(responses, SERVER_RESPONSE_OVER, response_count) != -1)
            over = true;
    }

    while (over == false)
    {
        response_count = receive_server_response(sock, responses, response_count);
        if (get_response_index(responses, SERVER_RESPONSE_OVER, response_count) != -1)
            over = true;
    }

    free_all_responses(responses, &response_count);
    log_printf(PRINT_INFORMATION, "[Brain][7] - Quitte\n");

}

void scan_for_resource(int sock, char RESPONSES_TAB, int *response_count, char *resource_name)
{
    int resource_position = -1;
    int player_level = 1;
    // printf("debut du scan LOOK\n");
    while (resource_position == -1)
    {
        int rotation = -1;
        while (++rotation < 4 && resource_position == -1)
        {
            // printf("!\n");
            resource_position = look(sock, responses, response_count, resource_name, &player_level);

            // printf("[%d] - [%d]\n", resource_position, player_level);
            //  printf("??\n");
        }
        // printf("---fini de LOOK\n");
        if (resource_position == -1)
            move_next_spot(sock, player_level, responses, response_count);
        else
        {
            go_to_cell(resource_position, sock, responses, response_count);
            // printf("deplacement done\n");
            char buffer[BUFFER_SIZE_TINY];
            sprintf(buffer, "prend %s\n", resource_name);
            // printf("avant pickup\n");
            execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
            // printf("Fini scan + pickup\n");
        }
    }
}

int execute_action(int sock, char *action, char RESPONSES_TAB, int *response_count, server_response_type_t response_type, bool delete)
{
    // printf("\n-- DEbut execute --\n");
    send_message(sock, action);
    int response_index = -1;
    while (response_index == -1)
    {
        // printf("debut boucle\n");
        // printf("DEBUT Boucle\n");
        //  printf("RC[%d]\n", *response_count);
        //  printf("1[0] = %s\n", responses[0]);
        //  printf("1[1] = %s\n", responses[1]);
        *response_count = receive_server_response(sock, responses, *response_count);
        // print_responses(responses, *response_count);
        //  printf("RC[%d]\n", *response_count);
        //  printf("2[0] = %s\n", responses[0]);
        //  printf("2[1] = %s\n", responses[1]);
        //  printf("response_count = %d | type = %d\n", *response_count, response_type);
        response_index = get_response_index(responses, response_type, *response_count);
        // printf("response_index = %d\n", response_index);
    }
    // printf("-- fiin execute --\n");
    if (delete)
        delete_response(responses, response_count, response_index);
    return response_index;
}

int look(int sock, char RESPONSES_TAB, int *response_count, char *resource_name, int *player_level)
{
    // printf("--- JE LOOK\n");
    int response_index = execute_action(sock, "voir\n", responses, response_count, SERVER_RESPONSE_OBJECT, false);
    // printf("--- [%s] \n", responses[response_index]);
    char cells[8 * 8][BUFFER_SIZE];
    //printf("[%s]\n", responses[response_index]);
    int cells_number = get_view(responses[response_index], cells);
    //printf("[%d]\n", cells_number);

    delete_response(responses, response_count, response_index);
    *player_level = (int)(sqrt(cells_number) - 1);
    return get_resource_position(cells, cells_number, resource_name);
}

void move_next_spot(int sock, int player_level, char RESPONSES_TAB, int *response_count)
{
    int forward_number = (player_level) * 2 + 1;

    for (int i = 0; i < forward_number; i++)
    {
        // printf("--- JE AVANCE\n");
        execute_action(sock, "avance\n", responses, response_count, SERVER_RESPONSE_OK_KO, true);
    }
    // printf("--- JE ROTATE\n");
    execute_action(sock, "droite\n", responses, response_count, SERVER_RESPONSE_OK_KO, true);
    for (int i = 0; i < (int)ceil(forward_number / 2); i++)
    {
        // printf("--- JE AVANCE\n");
        execute_action(sock, "avance\n", responses, response_count, SERVER_RESPONSE_OK_KO, true);
    }
}

void go_to_cell(int resource_position, int sock, char RESPONSES_TAB, int *response_count)
{
    if (resource_position == 0)
        return;
    int number_forward = (resource_position / 4) + 1;

    // Avance initiale
    for (int i = 0; i < number_forward; i++)
        execute_action(sock, "avance\n", responses, response_count, SERVER_RESPONSE_OK_KO, true);

    // Détermination de la rotation et du mouvement supplémentaire
    int mod = resource_position % 4; // Correction ici

    if (mod == 1)
    {
        execute_action(sock, "gauche\n", responses, response_count, SERVER_RESPONSE_OK_KO, true); // Tourne à gauche
        for (int i = 0; i < number_forward; i++)
            execute_action(sock, "avance\n", responses, response_count, SERVER_RESPONSE_OK_KO, true);
    }
    else if (mod == 3 || mod == 0)
    {
        execute_action(sock, "droite\n", responses, response_count, SERVER_RESPONSE_OK_KO, true); // Tourne à droite
        for (int i = 0; i < number_forward; i++)
            execute_action(sock, "avance\n", responses, response_count, SERVER_RESPONSE_OK_KO, true);
    }
}

void broadcast_mission(int sock, char RESPONSES_TAB, int *response_count, char *team_name)
{
    static int mission_index = 1;

    if (mission_index > 6)
        mission_index = 1;
    char buffer[BUFFER_SIZE_SMALL];
   // printf("[%d]SEND MISSION[%s][%d]\n", mission_index, resource_names[mission_index], resource_total_needed[mission_index]);

    char trim_team_name[BUFFER_SIZE_TINY];
    strcpy(trim_team_name, team_name);
    trim_team_name[strlen(trim_team_name) - 1] = '\0';
    sprintf(buffer, "broadcast %s %s %d mission\n", trim_team_name, resource_names[mission_index], resource_total_needed[mission_index]);
    // printf("execute avtion : [%s]\n", buffer);
    execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
    // printf("FIN d'action\n");
    mission_index++;
}

int inventory(int sock, char RESPONSES_TAB, int *response_count, char *resource_name)
{
    int response_index = execute_action(sock, "inventaire\n", responses, response_count, SERVER_RESPONSE_OBJECT, false);
    int quantity = get_item_amount(responses[response_index], resource_name);
    // printf("[%s]\n", responses[response_index]);
    delete_response(responses, response_count, response_index);

    // printf("QUQNTITYTYTYTYTYTY %d\n", quantity);
    return quantity;
}

int how_much_players(int sock, char RESPONSES_TAB, int *response_count)
{
    int response_index = execute_action(sock, "voir\n", responses, response_count, SERVER_RESPONSE_OBJECT, false);
    int quantity = get_players_count(responses[response_index]);
    delete_response(responses, response_count, response_index);
    return quantity;
}

void broadcast_beacon(int sock, char RESPONSES_TAB, int *response_count, char *team_name)
{
    char buffer[BUFFER_SIZE_SMALL];
    char trim_team_name[BUFFER_SIZE_TINY];
    strcpy(trim_team_name, team_name);
    trim_team_name[strlen(trim_team_name) - 1] = '\0';
    sprintf(buffer, "broadcast %s ici\n", trim_team_name);
    execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
}

bool enough_resources(int sock, char RESPONSES_TAB, int *response_count, int required_resources[6])
{
    int response_index = execute_action(sock, "voir\n", responses, response_count, SERVER_RESPONSE_OBJECT, false);
    bool enough_resources = is_item_enough(responses[response_index], required_resources);
    delete_response(responses, response_count, response_index);
    return enough_resources;
}
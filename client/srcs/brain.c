#include "../includes/client.h"

void brain(int sock, client_config_t config)
{
    char RESPONSES_TAB;
    int response_count = 0;

    for (int i = 0; i < 7; i++)
    {
        scan_for_resource(sock, responses, &response_count, "nourriture");
        execute_action(sock, "fork\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
    }
    while (1)
    {
        execute_action(sock, "gauche\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        execute_action(sock, "gauche\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
        scan_for_resource(sock, responses, &response_count, "nourriture");
        print_responses(responses, response_count);

        if (did_egg_hatched(responses, &response_count) == true)
            start_slave(config);
    }
    
}



void scan_for_resource(int sock, char RESPONSES_TAB, int *response_count, char *resource_name)
{
    int resource_position = -1;
    int player_level = 1;
    //printf("debut du scan LOOK\n");
    while (resource_position == -1)
    {
        int rotation = -1;
        while (++rotation < 4 && resource_position == -1){
            
            resource_position = look(sock, responses, response_count, resource_name, &player_level);
            //printf("[%d] - [%d]\n", resource_position, player_level);
        }
        //printf("---fini de LOOK\n"); 
        if (resource_position == -1)
            move_next_spot(sock, player_level, responses, response_count);
        else
        {
            go_to_cell(resource_position, sock, responses, response_count);
           // printf("deplacement done\n");
            char buffer[BUFFER_SIZE_TINY];
            sprintf(buffer, "prend %s\n", resource_name);
            //printf("avant pickup\n");
            execute_action(sock, buffer, responses, response_count, SERVER_RESPONSE_OK_KO, true);
            //printf("Fini scan + pickup\n");    
        }
    }   
}

int execute_action(int sock, char *action, char RESPONSES_TAB, int *response_count, server_response_type_t response_type, bool delete)
{
    //printf("\n-- DEbut execute --\n");
    send_message(sock, action);
    int response_index = -1;
    while (response_index == -1)
    {
        //printf("DEBUT Boucle\n");
        // printf("RC[%d]\n", *response_count);
        // printf("1[0] = %s\n", responses[0]);
        // printf("1[1] = %s\n", responses[1]);
        *response_count = receive_server_response(sock, responses, *response_count);
        // printf("RC[%d]\n", *response_count);
        // printf("2[0] = %s\n", responses[0]);
        // printf("2[1] = %s\n", responses[1]);
        // printf("response_count = %d | type = %d\n", *response_count, response_type);
        response_index = get_response_index(responses, response_type, *response_count);
        //printf("response_index = %d\n", response_index);
    }
    //printf("-- fiin execute --\n");
    if (delete)
        delete_response(responses, response_count, response_index);
    return response_index;
}

int look(int sock, char RESPONSES_TAB, int *response_count, char *resource_name, int *player_level)
{
    //printf("--- JE LOOK\n");
    int response_index = execute_action(sock, "voir\n", responses, response_count, SERVER_RESPONSE_OBJECT, false);
    //printf("--- [%s] \n", responses[response_index]);
    char cells[8 * 8][BUFFER_SIZE];
    int cells_number = get_view(responses[response_index], cells);
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
    //printf("--- JE ROTATE\n");
    execute_action(sock, "droite\n", responses, response_count, SERVER_RESPONSE_OK_KO, true);
    for (int i = 0; i < (int)ceil(forward_number / 2); i++)
    {
        //printf("--- JE AVANCE\n");
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

bool did_egg_hatched(char RESPONSES_TAB, int *response_count)
{
   for (int i = 0; i < *response_count; i++)
   {
       if (strstr(responses[i], "egg hatched") != NULL)
       {
           delete_response(responses, response_count, i);
           return true;
       }
   }
   return false;
}


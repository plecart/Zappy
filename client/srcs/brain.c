#include "../includes/client.h"

void brain(int sock)
{
    char RESPONSES_TAB;
    int response_count = 0;

    // response_count = receive_server_response(sock, responses, response_count);
    while (1)
    {
        scan_for_resource(sock, responses, &response_count, "nourriture");
    }
}

void scan_for_resource(int sock, char RESPONSES_TAB, int *response_count, char *resource_name)
{
    int resource_position = -1;
    int player_level = 1;
    while (resource_position == -1)
    {
        int rotation = 0;
        int response_index = -1;
        while (rotation < 4 && resource_position == -1)
            resource_position = look_around(sock, responses, response_count, resource_name, &player_level);
        if (resource_position == -1)
            move_next_spot(sock, player_level, responses, &response_count);
        else
            printf("----\n");
    }
}

int execute_action(int sock, char *action, char RESPONSES_TAB, int *response_count, server_response_type_t response_type, bool delete)
{
    send_message(sock, action);
    int response_index = -1;
    while ((response_index = get_response_index(responses, response_type, *response_count)) == -1)
        *response_count = receive_server_response(sock, responses, *response_count);
    if (delete)
        delete_response(responses, response_count, response_index);
    return response_index;
}

int look_around(int sock, char RESPONSES_TAB, int *response_count, char *resource_name, int *player_level)
{
    printf("--- JE LOOK\n");
    int response_index = execute_action(sock, "voir\n", responses, &response_count, SERVER_RESPONSE_OBJECT, false);
    printf("--- [%s] \n", responses[response_index]);
    char cells[8 * 8][BUFFER_SIZE];
    int cells_number = get_view(responses[response_index], cells);
    delete_response(responses, response_count, response_index);
    player_level = sqrt(cells_number) - 1;
    return get_resource_position(cells, cells_number, resource_name);
}

int get_view(char responses[BUFFER_SIZE_SMALL], char cells[8 * 8][BUFFER_SIZE])
{
    char *token = strtok(responses, ",");
    int j = 0;
    while (token != NULL)
    {
        strcpy(cells[j], token);
        token = strtok(NULL, ",");
        j++;
    }
    return j;
}

int get_resource_position(char cells[8 * 8][BUFFER_SIZE], int cells_number, char *resource_name)
{
    for (int i = 0; i < cells_number; i++)
        if (strstr(cells[i], resource_name) != NULL)
            return i;
    return -1;
}

void move_next_spot(int sock, int player_level, char RESPONSES_TAB, int *response_count)
{
    int forward_number = (player_level) * 2 + 1;

    for (int i = 0; i < forward_number; i++)
    {
        printf("--- JE AVANCE\n");
        execute_action(sock, "avance\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
    }
    printf("--- JE ROTATE\n");
    execute_action(sock, "droite\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);

    for (int i = 0; i < (int)ceil(forward_number / 2); i++)
    {
        printf("--- JE AVANCE\n");
        execute_action(sock, "avance\n", responses, &response_count, SERVER_RESPONSE_OK_KO, true);
    }
}

void go_to_cell(int resource_position, int sock, char RESPONSES_TAB, int *response_count)
{
    int number_forward = (resource_position / 4) + 1;

    // Avance initiale
    for (int i = 0; i < number_forward; i++)
        execute_action(sock, "avance\n", RESPONSES_TAB, response_count, SERVER_RESPONSE_OK_KO, true);

    // Détermination de la rotation et du mouvement supplémentaire
    int mod = resource_position % 4; // Correction ici

    if (mod == 1)
    {
        execute_action(sock, "gauche\n", RESPONSES_TAB, response_count, SERVER_RESPONSE_OK_KO, true); // Tourne à gauche
        for (int i = 0; i < number_forward; i++)
            execute_action(sock, "avance\n", RESPONSES_TAB, response_count, SERVER_RESPONSE_OK_KO, true);
    }
    else if (mod == 3 || mod == 0)
    {
        execute_action(sock, "droite\n", RESPONSES_TAB, response_count, SERVER_RESPONSE_OK_KO, true); // Tourne à droite
        for (int i = 0; i < number_forward; i++)
            execute_action(sock, "avance\n", RESPONSES_TAB, response_count, SERVER_RESPONSE_OK_KO, true);
    }
}


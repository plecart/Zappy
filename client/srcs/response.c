#include "../includes/client.h"

int get_response_index(char RESPONSES_TAB, server_response_type_t type, int response_count)
{
    for (int i = 0; i < response_count; i++)
    {
        if (type == SERVER_RESPONSE_OK_KO && (strcmp(responses[i], "ok") == 0 || strcmp(responses[i], "ko") == 0))
            return i;

        if (type == SERVER_RESPONSE_OBJECT && responses[i][0] == '{')
            return i;

        if (type == SERVER_RESPONSE_BEACON && strstr(responses[i], "ici") != NULL)
            return i;

        if (type == SERVER_RESPONSE_INCANTATION && strstr(responses[i], "elevation") != NULL)
            return i;

        if (type == SERVER_RESPONSE_OVER && (strstr(responses[i], "mort") != NULL || strstr(responses[i], "fini") != NULL))
            return i;
    }
    return -1;
};

void delete_response(char RESPONSES_TAB, int *response_count, int index)
{
    if (index < 0 || index >= *response_count)
    {
        log_printf(PRINT_ERROR, "Index hors des limites dans delete_response\n");
        return;
    }

    free(responses[index]);
    for (int j = index; j < *response_count - 1; j++)
    {
        responses[j] = responses[j + 1];
    }
    (*response_count)--;
    responses[*response_count] = NULL;
}

void filter_responses(char RESPONSES_TAB, int *response_count, client_config_t config, bool is_slave)
{

    for (int i = 0; i < *response_count; i++)
    {
        bool is_message = is_broadcast_team(responses[i], config.team_name);
        if (strcmp(responses[i], "BIENVENUE") == 0 ||
            is_coordinate(responses[i]) ||
            is_message == false ||
            (is_message && is_slave && strstr(responses[i], "done") != NULL) ||
            (is_message && !is_slave && strstr(responses[i], "mission") != NULL))
        {
            delete_response(responses, response_count, i);
            i--;
        }
    }
}

void filter_slaves_extra_responses(char RESPONSES_TAB, int *response_count)
{
    int last_done_index = -1;

    for (int i = 0; i < *response_count; i++)
    {
        if (strstr(responses[i], "mission") != NULL)
        {
            delete_response(responses, response_count, i);
            i--;
        }
        else if (strstr(responses[i], "ici") != NULL)
        {
            last_done_index = i;
        }
    }

    if (last_done_index != -1)
    {
        for (int i = 0; i < last_done_index; i++)
        {
            if (strstr(responses[i], "ici") != NULL)
            {
                delete_response(responses, response_count, i);
                i--;
            }
        }
    }
}

void print_responses(char RESPONSES_TAB, int response_count)
{
    if (response_count <= 0)
        return;
    printf(" - - - - - - - - \n");
    for (int i = 0; i < response_count; i++)
    {
        printf("REPONSE[%d] = [%s]\n", i, responses[i]);
    }
    printf(" - - - - - - - - \n");
}

void free_all_responses(char *responses[], int *response_count)
{
    int i = 0;
    while (i < *response_count)

    {
        delete_response(responses, response_count, i);
    }
}

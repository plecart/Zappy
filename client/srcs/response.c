#include "../includes/client.h"

int get_response_index(char RESPONSES_TAB, server_response_type_t type, int response_count)
{
    for (int i = 0; i < response_count; i++)
    {
        // printf("[%s]\n", responses[i]);
        if (type == SERVER_RESPONSE_OK_KO && (strcmp(responses[i], "ok") == 0 || strcmp(responses[i], "ko") == 0))
            return i;
        // printf("NON\n");
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
    // Vérification de l'index pour éviter les erreurs de dépassement
    if (index < 0 || index >= *response_count)
    {
        log_printf(PRINT_ERROR, "Index hors des limites dans delete_response\n");
        return;
    }

    // Libération de la mémoire allouée pour la réponse à supprimer
    free(responses[index]);

    // Décalage des réponses restantes pour combler l'espace
    for (int j = index; j < *response_count - 1; j++)
    {
        responses[j] = responses[j + 1];
    }

    // Réduire le nombre de réponses
    (*response_count)--;

    // S'assurer que la dernière réponse est NULL et libérer l'espace
    responses[*response_count] = NULL;
}

void filter_responses(char RESPONSES_TAB, int *response_count, client_config_t config, bool is_slave)
{

    for (int i = 0; i < *response_count; i++)
    {
        // printf("- - -REPONSE[%d] = [%s]\n", i, responses[i]);
        bool is_message = is_broadcast_team(responses[i], config.team_name);
        // printf("id message = [%d] | is_slave == [%d] | res = [%s]\n", is_message, is_slave, responses[i]);
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

    // Supprimer les réponses contenant "mission" et repérer la dernière occurrence de "done"
    for (int i = 0; i < *response_count; i++)
    {
        if (strstr(responses[i], "mission") != NULL)
        {
            delete_response(responses, response_count, i);
            i--; // Réajuster l'index après suppression
        }
        else if (strstr(responses[i], "ici") != NULL)
        {
            last_done_index = i;
        }
    }

    // Supprimer toutes les occurrences de "done" sauf la dernière
    if (last_done_index != -1)
    {
        for (int i = 0; i < last_done_index; i++)
        {
            if (strstr(responses[i], "ici") != NULL)
            {
                delete_response(responses, response_count, i);
                i--; // Réajuster l'index après suppression
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

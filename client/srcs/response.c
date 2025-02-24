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

void filter_responses(char RESPONSES_TAB, int *response_count, client_config_t config)
{
    for (int i = 0; i < *response_count; i++)
    {
       // printf("- - -REPONSE[%d] = [%s]\n", i, responses[i]);
        if (strcmp(responses[i], "BIENVENUE") == 0 ||
            is_coordinate(responses[i]) || is_broadcast_team(responses[i], config.team_name) == false
        )
        {
            delete_response(responses, response_count, i);
            i--;
        }
    }
}

void print_responses(char RESPONSES_TAB, int response_count)
{
    for (int i = 0; i < response_count; i++)
    {
        printf("REPONSE[%d] = [%s]\n", i, responses[i]);
    }
}
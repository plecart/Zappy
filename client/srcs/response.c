#include "../includes/client.h"


int get_response_index(char RESPONSES_TAB, server_response_type_t type, int response_count)
{
    for (int i = 0; i < response_count; i++)
    {
        printf("[%s]\n", responses[i]);
        if (type == SERVER_RESPONSE_OK_KO && (strcmp(responses[i], "ok") == 0 || strcmp(responses[i], "ko") == 0))
            return i;
        printf("NON\n");
        if (type == SERVER_RESPONSE_OBJECT && responses[i][0] == '{')
            return i;
    }
    return -1;
};

void delete_response(char RESPONSES_TAB, int *response_count, int index)
{printf("RETIR ORDRE -------- DELTE [%d][%d]RC\n", index, *response_count);
    for (int j = index; j < *response_count - 1; j++)
        strcpy(responses[j], responses[j + 1]);
    (*response_count)--;
    responses[*response_count][0] = '\0';
}
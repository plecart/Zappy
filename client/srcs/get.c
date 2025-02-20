#include "../includes/client.h"

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
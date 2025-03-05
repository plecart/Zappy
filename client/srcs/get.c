#include "../includes/client.h"

const char *const resource_names[] = {NOURRITURE, LINEMATE, DERAUMERE, SIBUR, PHIRAS, MENDIANE, THYSTAME};
const int resource_total_needed[] = {0, 10, 9, 11, 6, 7, 2};

bool is_coordinate(char *str)
{
    int i = 0;

    if (!isdigit(str[i]))
        return false;
    while (isdigit(str[i]))
        i++;

    if (str[i] != ' ')
        return false;
    i++;

    if (!isdigit(str[i]))
        return false;
    while (isdigit(str[i]))
        i++;

    return true;
}

bool is_broadcast_team(const char *str, const char *team_name)
{
    if (!str || !team_name)
    {
        return false;
    }
    char trim_team_name[BUFFER_SIZE_TINY];
    strcpy(trim_team_name, team_name);
    trim_team_name[strlen(trim_team_name) - 1] = '\0';
    static const char prefix[] = "message ";
    size_t prefix_len = sizeof(prefix) - 1;
    if (strncmp(str, prefix, prefix_len) != 0)
    {
        return false;
    }
    str += prefix_len;
    if (!isdigit((unsigned char)*str))
    {
        return false;
    }
    str++;
    if (*str != ',' || *(str + 1) != ' ')
    {
        return false;
    }
    str += 2;
    size_t team_len = strlen(trim_team_name);
    if (strncmp(str, trim_team_name, team_len - 1) != 0)
    {
        return false;
    }
    str += team_len;
    return true;
}

int get_mission(const char *str, const char *team_name, char *mission)
{
    if (!str || !team_name)
    {
        return -1;
    }
    const char *p = strstr(str, team_name);
    if (!p)
    {
        return -1;
    }
    p += strlen(team_name);
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }
    int i = 0;
    while (*p && !isspace((unsigned char)*p) && i < (int)BUFFER_SIZE_TINY - 1)
    {
        mission[i++] = *p++;
    }
    mission[i] = '\0';
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }
    int value = atoi(p);
    return value;
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

int get_resource_position(char cells[8 * 8 + 3][BUFFER_SIZE], int cells_number, char *resource_name)
{
    for (int i = 0; i < cells_number; i++)
        if (strstr(cells[i], resource_name) != NULL)
            return i;
    return -1;
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

int get_item_amount(char *inventory, char *resource_name)
{
    char *found = strstr(inventory, resource_name);
    if (!found)
    {
        return -1;
    }
    found += strlen(resource_name);
    while (*found && *found != ':')
        found++;
    if (!*found)
    {
        return -1;
    }
    found++;
    while (*found == ' ' || *found == '\t')
        found++;
    int amount = 0;
    sscanf(found, "%d", &amount);

    return amount;
}

int get_index_ressource(const char *resource)
{
    if (!resource)
        return -1;

    for (int i = 0; i < 7; i++)
    {
        size_t len = strlen(resource_names[i]);
        if (strncmp(resource, resource_names[i], len) == 0)
            return i;
    }
    return -1;
}

int is_slave_ready(char RESPONSES_TAB, int *response_count)
{
    int total_ready = 0;
    int i = 0;
    while (i < *response_count)
    {
        if (strstr(responses[i], "done") != NULL)
        {
            delete_response(responses, response_count, i);
            total_ready++;
            i = 0;
        }
        i++;
    }
    return total_ready;
}

int get_players_count(const char *str)
{
    const char *comma_pos = strchr(str, ',');
    int count = 0;
    size_t length = comma_pos ? (size_t)(comma_pos - str) : strlen(str);
    char *temp = malloc(length + 1);
    if (!temp)
        return -1;
    memcpy(temp, str, length);
    temp[length] = '\0';
    char *token = strtok(temp, " \t\n{}");
    while (token)
    {
        if (strcmp(token, "joueur") == 0)
        {
            count++;
        }
        token = strtok(NULL, " \t\n");
    }
    free(temp);
    return count;
}

int get_message_direction(const char *str)
{
    const char *p = strstr(str, "message ");
    if (p == NULL)
    {
        return -1;
    }
    p += strlen("message ");
    if (!isdigit((unsigned char)*p))
    {
        return -1;
    }

    return *p - '0';
}

bool is_item_enough(const char *view, const int required_resources[6])
{
    static const char *res_names[6] = {
        "linemate",
        "deraumere",
        "sibur",
        "mendiane",
        "phiras",
        "thystame"};
    int counts[6] = {0, 0, 0, 0, 0, 0};

    char buffer[2048];
    strncpy(buffer, view, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *comma_pos = strchr(buffer, ',');
    if (comma_pos != NULL)
    {
        *comma_pos = '\0';
    }

    char *token = strtok(buffer, " \t\n");
    while (token != NULL)
    {

        char *start = token;
        while (*start && !isalpha((unsigned char)*start))
            start++;

        char *end = token + strlen(token) - 1;
        while (end > start && !isalpha((unsigned char)*end))
        {
            *end = '\0';
            end--;
        }

        if (start != token)
        {
            memmove(token, start, strlen(start) + 1);
        }

        for (int i = 0; i < 6; i++)
        {
            if (strcmp(token, res_names[i]) == 0)
            {
                counts[i]++;
                break;
            }
        }

        token = strtok(NULL, " \t\n");
    }

    for (int i = 0; i < 6; i++)
    {
        if (counts[i] < required_resources[i])
        {
            return false;
        }
    }
    return true;
}
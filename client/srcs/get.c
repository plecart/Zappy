#include "../includes/client.h"

const char *const resource_names[] = {NOURRITURE, LINEMATE, DERAUMERE, SIBUR, PHIRAS, MENDIANE, THYSTAME};
const int  resource_total_needed[] = {0, 9, 8, 10, 5, 6, 1};

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
    //  printf("=====> %s - %s\n", str, team_name);
    if (!str || !team_name)
    {
        return false;
    }

    //printf("TEAMNAME = [%s]\n", team_name);
    char trim_team_name[BUFFER_SIZE_TINY];
    strcpy(trim_team_name, team_name);
    trim_team_name[strlen(trim_team_name) - 1] = '\0';
    //printf("TRIM = [%s]\n---\n", trim_team_name);

    // Format attendu minimum : "message <digit>, <team_name>"
    static const char prefix[] = "message ";
    size_t prefix_len = sizeof(prefix) - 1; // équivalent à strlen("message ")

    // 1) Vérifier que la chaîne commence par "message "
    //printf("CMP => [%s] - [%s] [%ld]\n", str, prefix, prefix_len);
    if (strncmp(str, prefix, prefix_len) != 0)
    {
        return false;
    }
    str += prefix_len; // avancer le pointeur

    // 2) Vérifier qu'il y a un et un seul chiffre
    //    (si vous voulez autoriser plusieurs chiffres, adaptez cette partie)
    //printf("2\n");
    if (!isdigit((unsigned char)*str))
    {
        return false;
    }
    str++; // avancer après le chiffre
           // printf("1\n");
    // 3) Vérifier qu'il y a ", "
   // printf("3\n");
    if (*str != ',' || *(str + 1) != ' ')
    {
        return false;
    }
    str += 2; // avancer après ", "
              // printf("2\n");
    // 4) Vérifier le nom de l'équipe

    size_t team_len = strlen(trim_team_name);
    // printf("==/===> [%s] - [%s] [%ld]\n", str, trim_team_name, team_len);
   // printf("4\n");
   // printf("==/===> [%s] - [%s] [%ld]\n", str, trim_team_name, team_len);
    if (strncmp(str, trim_team_name, team_len - 1) != 0)
    {
        return false;
    }
    // printf("3\n");
    str += team_len; // avancer après team_name

    // À partir d'ici, on ne fait plus AUCUNE vérification !
    // Qu'il y ait un espace, du texte, etc. on ignore tout.
    // Retourner true pour indiquer que tout le "préfixe" correspond au format attendu.
   // printf("TRUR\n");
    return true;
}

int get_mission(const char *str, const char *team_name, char *mission)
{
    // 1) Trouver la sous-chaîne "Team2" (ou team_name) dans str.
    //    (On suppose qu'elle existe et qu'elle n'est pas NULL, car la chaîne est déjà validée.)
    printf("===> %s - %s|\n", str, team_name);
    if (!str || !team_name)
    {
        return -1;
    }

    printf("===> [%s] - [%s] ????\n", str, team_name);
    const char *p = strstr(str, team_name);
    if (!p)
    {
        // Si jamais vous vouliez gérer un cas d'erreur...
        return -1;
    }
    // printf("1\n");
    // Avancer de la longueur de team_name
    p += strlen(team_name);

    // 2) Sauter les espaces éventuellement présents après "Team2"
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }

    // 3) Récupérer le mot (mission) qui se trouve juste à droite.
    //    On copie caractère par caractère jusqu'à retomber sur un espace ou la fin de chaîne.
    int i = 0;
    while (*p && !isspace((unsigned char)*p) && i < (int)BUFFER_SIZE_TINY - 1)
    {
        mission[i++] = *p++;
    }
    mission[i] = '\0'; // On termine la chaîne mission proprement.

    // 4) Sauter les espaces avant le nombre
    while (*p && isspace((unsigned char)*p))
    {
        p++;
    }

    // 5) Convertir en entier ce qu'il reste (on suppose que c'est un nombre).
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
    // Look for the resource name in the inventory string
    char *found = strstr(inventory, resource_name);
    if (!found)
    {
        // If not found, return 0 or handle the error as you prefer
        return -1;
    }

    // Advance the pointer to the end of the resource name
    found += strlen(resource_name);

    // Find the colon that follows the resource name
    while (*found && *found != ':')
        found++;
    if (!*found)
    {
        // If we didn't find a colon, something is malformed
        return -1;
    }
    // Skip the colon
    found++;

    // Skip any whitespace
    while (*found == ' ' || *found == '\t')
        found++;

    // Parse the integer after the colon
    int amount = 0;
    sscanf(found, "%d", &amount);

    return amount;
}

int get_index_ressource(const char *resource)
{
    if (!resource)
        return -1;

    for (int i = 0; i < 7; i++) {
        size_t len = strlen(resource_names[i]);
        // Compare seulement les 'len' premiers caractères
        // de 'resource' avec 'resource_names[i]'.
        if (strncmp(resource, resource_names[i], len) == 0)
            return i;
    }
    return -1;
}

int is_slave_ready(char RESPONSES_TAB, int *response_count)
{
    int total_ready = 0;
    for (int i = 0; i < *response_count; i++)
    {
        if (strstr(responses[i], "done") != NULL)
        {
            delete_response(responses, response_count, i);
            total_ready++;
            i = 0;
        }
    }
    return total_ready;
}

int get_players_count(const char *str)
{
    const char *comma_pos = strchr(str, ',');
    int count = 0;

    // Si on trouve une virgule, on ne traitera que la partie avant celle-ci
    size_t length = comma_pos ? (size_t)(comma_pos - str) : strlen(str);

    // On recopie cette portion dans un buffer temporaire pour la tokeniser
    char *temp = malloc(length + 1);
    if (!temp)
        return -1; // erreur d'allocation

    memcpy(temp, str, length);
    temp[length] = '\0';

    // Tokenisation par espaces
    char *token = strtok(temp, " \t\n");
    while (token) {
        if (strcmp(token, "joueur") == 0) {
            count++;
        }
        token = strtok(NULL, " \t\n");
    }

    free(temp);
    return count;
}

int get_message_direction(const char *str)
{
    // On cherche la sous-chaîne "message "
    const char *p = strstr(str, "message ");
    if (p == NULL) {
        return -1; // Pas trouvé
    }
    // On avance le pointeur juste après "message "
    p += strlen("message ");

    // On vérifie que le caractère suivant est bien un chiffre
    if (!isdigit((unsigned char)*p)) {
        return -1; // Pas un chiffre
    }

    // On convertit le caractère en entier (par exemple '3' -> 3)
    return *p - '0';
}
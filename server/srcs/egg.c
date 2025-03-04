#include "../includes/server.h"

void add_egg(egg_t *eggs[], int *egg_count, player_t *player)
{

    egg_t *new_egg = malloc(sizeof(egg_t)); // ✅ Allouer la mémoire correctement
    if (!new_egg)
    {
        log_printf(PRINT_ERROR, "Échec de l'allocation mémoire pour un œuf\n");
        return ;
    }

    static int egg_id = 0;
    
    strcpy(new_egg->team_name, player->team_name);
    new_egg->mother_socket = player->socket;
    new_egg->x = player->x;
    new_egg->y = player->y;
    new_egg->time_before_spawn = 42;  // remettre 42
    new_egg->time_before_hatch = 600; // remettre 600
    new_egg->id = egg_id++;
    
    eggs[*egg_count] = new_egg; // ✅ Copy the new egg
    (*egg_count)++;
}

void remove_egg(egg_t *eggs[], int *egg_count, const char *team_name, int x, int y)
{
    for (int i = 0; i < *egg_count; i++)
    {
        if (eggs[i] != NULL && strcmp(eggs[i]->team_name, team_name) == 0 &&
            eggs[i]->x == x && eggs[i]->y == y && eggs[i]->time_before_hatch == 0)
        {
            // ✅ Libération de la mémoire de l'œuf supprimé
            free(eggs[i]);

            // ✅ Décalage des éléments restants pour combler le trou
            for (int j = i; j < *egg_count - 1; j++)
            {
                eggs[j] = eggs[j + 1];
            }

            // ✅ Mise à NULL du dernier élément pour éviter une référence invalide
            eggs[*egg_count - 1] = NULL;
            (*egg_count)--;

            return;
        }
    }
    log_printf(PRINT_ERROR, "Aucun œuf à retirer\n");
}

void add_egg_cycle(int graphic_socket, egg_t *eggs[], int egg_count)
{
    for (int i = 0; i < egg_count; i++)
    {
        if (eggs[i] == NULL) // ✅ Vérification ajoutée
            continue;
        // printf("egg %d\n", i);
        if (eggs[i]->time_before_spawn > 0)
        {
            eggs[i]->time_before_spawn--;
            if (eggs[i]->time_before_spawn == 0)
            {
                log_printf(PRINT_INFORMATION, "Un oeuf de l'équipe [%s] est apparu en [%d, %d] (posé par %d)\n",
                           eggs[i]->team_name, eggs[i]->x, eggs[i]->y, eggs[i]->mother_socket);
                send_graphic_egg_created(graphic_socket, i, eggs[i]);
            }
        }
        else if (eggs[i]->time_before_hatch > 0)
        {
            eggs[i]->time_before_hatch--;
            if (eggs[i]->time_before_hatch == 0)
            {
                log_printf(PRINT_INFORMATION, "Un oeuf de l'équipe [%s] a éclos en [%d, %d] (en attente d'un nouveau client)\n",
                           eggs[i]->team_name, eggs[i]->x, eggs[i]->y, eggs[i]->mother_socket);
                send_message_egg(*eggs[i], "egg hatched\n");
                send_graphic_egg_hatched(graphic_socket, eggs[i]->id);
            }
        }
    }
}

void free_eggs(egg_t *eggs[], int egg_count)
{
    for (int i = 0; i < egg_count; i++) {
        free(eggs[i]);
        eggs[i] = NULL;
    }
}
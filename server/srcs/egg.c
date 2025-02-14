#include "../includes/server.h"

void add_egg(egg_t **eggs, size_t *egg_count, egg_t new_egg)
{
    *eggs = realloc(*eggs, (*egg_count + 1) * sizeof(egg_t));
    if (!*eggs)
    {
        log_printf(PRINT_ERROR, "Erreur de la re-allocation d'un nouvel oeuf'\n");
        exit(EXIT_FAILURE);
    }
    (*eggs)[*egg_count] = new_egg;
    (*egg_count)++;
}

void remove_egg(egg_t **eggs, size_t *egg_count, const char *team_name, int x, int y)
{
    for (size_t i = 0; i < *egg_count; i++)
    {
        if (strcmp((*eggs)[i].team_name, team_name) == 0 && (*eggs)[i].x == x && (*eggs)[i].y == y && (*eggs)[i].time_before_hatch == 0)
        {
            for (size_t j = i; j < *egg_count - 1; j++)
            {
                (*eggs)[j] = (*eggs)[j + 1];
            }
            (*egg_count)--;
            *eggs = realloc(*eggs, *egg_count * sizeof(egg_t));
            if (*egg_count > 0 && !*eggs)
            {
                log_printf(PRINT_ERROR, "Erreur de la re-allocation pour la suppression d'un oeuf'\n");
                exit(EXIT_FAILURE);
            }
            return;
        }
    }
    log_printf(PRINT_ERROR, "Aucun oeuf a retirer'\n");
}

void free_egg_array(egg_t **eggs, size_t *egg_count)
{
    free(*eggs);
    *eggs = NULL;
    *egg_count = 0;
}

void add_egg_cycle(egg_t *eggs, size_t egg_count)
{
    for (size_t i = 0; i < egg_count; i++)
    {
        if (eggs[i].time_before_spawn > 0)
        {
            eggs[i].time_before_spawn--;
            if (eggs[i].time_before_spawn == 0)
            {
                log_printf(PRINT_INFORMATION, "Un oeuf de l'équipe [%s] est apparu en [%d, %d] (posé par %d)\n", eggs[i].team_name, eggs[i].x, eggs[i].y, eggs[i].mother_socket);
            }
        }
        else if (eggs[i].time_before_hatch > 0)
        {
            eggs[i].time_before_hatch--;
            if (eggs[i].time_before_hatch == 0)
            {

                log_printf(PRINT_INFORMATION, "Un oeuf de l'équipe [%s] a éclos en [%d, %d] (en attente d'un nouveau client)\n", eggs[i].team_name, eggs[i].x, eggs[i].y, eggs[i].mother_socket);

                send_message_egg(eggs[i], "egg hatched\n");
            }
        }
    }
}
#include "../includes/server.h"

map_t *create_map(int width, int height) {
    map_t *map = malloc(sizeof(map_t));
    if (!map) {
        log_printf("Erreur d'allocation de la carte\n");
        exit(EXIT_FAILURE);
    }
    map->width = width;
    map->height = height;


    map->cells = malloc(height * sizeof(cell_t *));
    if (!map->cells) {
        log_printf("Erreur d'allocation des cases de la carte\n");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < height; y++) {
        map->cells[y] = malloc(width * sizeof(cell_t));
        if (!map->cells[y]) {
            log_printf("Erreur d'allocation d'une ligne de la carte\n");
            exit(EXIT_FAILURE);
        }

        for (int x = 0; x < width; x++) {
            memset(&map->cells[y][x].resources, 0, sizeof(resources_t));
        }
    }

    return map;
}

void populate_map(map_t *map) {
    srand(time(NULL)); 

    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            map->cells[y][x].resources.nourriture = rand() % 10;
            map->cells[y][x].resources.linemate = rand() % 6;
            map->cells[y][x].resources.deraumere = rand() % 6;
            map->cells[y][x].resources.sibur = rand() % 6;
            map->cells[y][x].resources.mendiane = rand() % 4;
            map->cells[y][x].resources.phiras = rand() % 4;
            map->cells[y][x].resources.thystame = rand() % 2;
        }
    }
}

void print_map(map_t *map) {
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            printf("[%dN %dL %dD %dS %dM %dP %dT]\n",
                map->cells[y][x].resources.nourriture,
                map->cells[y][x].resources.linemate,
                map->cells[y][x].resources.deraumere,
                map->cells[y][x].resources.sibur,
                map->cells[y][x].resources.mendiane,
                map->cells[y][x].resources.phiras,
                map->cells[y][x].resources.thystame);
        }
        printf("\n");
    }
}

void free_map(map_t *map) {
    for (int y = 0; y < map->height; y++) {
        free(map->cells[y]);
    }
    free(map->cells);
    free(map);
}

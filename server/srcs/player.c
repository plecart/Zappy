#include "../includes/server.h"

void close_invalid_client(int client_socket, const char *message) {
    log_printf(PRINT_ERROR, "%s\n", message);
    close(client_socket);
}

int validate_team(const char *team_name, server_config_t *config) {
    for (int i = 0; i < config->team_count; i++) {
        if (strcmp(team_name, config->teams[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int count_players_in_team(player_t *players[], int max_players, const char *team_name) {
    int count = 0;
    for (int i = 0; i < max_players; i++) {
        if (players[i] != NULL && strcmp(players[i]->team_name, team_name) == 0) {
            count++;
        }
    }
    return count;
}

void assign_new_player(int client_socket, player_t *players[], int max_players, const char *team_name, server_config_t *config) {
    for (int i = 0; i < max_players; i++) {
        if (players[i] == NULL) {
            players[i] = malloc(sizeof(player_t));
            players[i]->socket = client_socket;
            strncpy(players[i]->team_name, team_name, sizeof(players[i]->team_name) - 1);
            players[i]->team_name[sizeof(players[i]->team_name) - 1] = '\0';
            players[i]->x = rand() % config->width;
            players[i]->y = rand() % config->height;
            
            log_printf(PRINT_INFORMATION, "Joueur assigné à l'équipe %s, position [%d, %d]\n", team_name, players[i]->x, players[i]->y);
            dprintf(client_socket, "%d %d\n", players[i]->x, players[i]->y);
            return;
        }
    }
    close_invalid_client(client_socket, "Trop de joueurs connectés");
}

void accept_new_client(int server_socket, player_t *players[], int max_players, server_config_t *config) {
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
    
    if (client_socket < 0) {
        log_printf(PRINT_ERROR, "Erreur lors de accept\n");
        return;
    }
    
    log_printf(PRINT_INFORMATION, "Nouvelle connexion acceptée (socket %d)\n", client_socket);
    
    char team_name[BUFFER_SIZE];
    read(client_socket, team_name, BUFFER_SIZE);
    team_name[strcspn(team_name, "\n")] = 0; // Retirer le saut de ligne
    
    if (!validate_team(team_name, config)) {
        close_invalid_client(client_socket, "Équipe invalide");
        return;
    }
    
    if (count_players_in_team(players, max_players, team_name) >= config->clients_per_team) {
        close_invalid_client(client_socket, "Équipe pleine");
        return;
    }
    
    assign_new_player(client_socket, players, max_players, team_name, config);
}

void free_players(player_t *players[], int max_clients) {
     for (int i = 0; i < max_clients; i++) {
        if (players[i] != NULL) {
            close(players[i]->socket);
            free(players[i]);
        }
    }
}

void print_players(player_t *players[], int max_players) {
    printf("- - - - - Liste des joueurs - - - - -\n");
    for (int i = 0; i < max_players; i++) {
        if (players[i] != NULL) {
            printf("Index: %d | Socket: %d | Team: %s | Position: [%d, %d]\n",
                       i, players[i]->socket, players[i]->team_name, players[i]->x, players[i]->y);
        } else {
            printf("Index: %d | Empty slot\n", i);
        }
    }
    printf("- - - - - - - - - - - - - - - - - - -\n");
}
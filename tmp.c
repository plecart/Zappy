#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>

#define BUFFER_SIZE_TINY 64
#define BUFFER_SIZE_SMALL 256 
#define BUFFER_SIZE_MEDIUM 512
#define BUFFER_SIZE 1024
#define BUFFER_SIZE_LARGE 2048

void log_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int connect_to_server(const char *hostname, int port) {
    int sock;
    struct sockaddr_in server_addr;

    // Création du socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur lors de la création du socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, hostname, &server_addr.sin_addr) <= 0) {
        perror("Adresse invalide");
        close(sock);
        return -1;
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Échec de la connexion");
        close(sock);
        return -1;
    }

    log_printf("Connecté au serveur %s:%d\n", hostname, port);
    return sock;
}

void receive_messages(int sock) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            log_printf("%s", buffer);

            if (strcmp(buffer, "La partie commence !\n") == 0) {
                log_printf("Détection du début de la partie !\n");
                break;
            }
        } else {
            log_printf("Déconnexion du serveur.\n");
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <adresse_serveur> <port>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sock = connect_to_server(server_ip, port);
    if (sock == -1) return 1;

    // Envoyer "GRAPHIC\n" au serveur
    write(sock, "GRAPHIC\n", 8);
    log_printf("Identifié comme client graphique.\n");

    // Attendre les messages du serveur
    receive_messages(sock);

    close(sock);
    return 0;
}

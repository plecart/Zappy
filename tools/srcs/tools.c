#include "../includes/tools.h"

const char *print_type_str[] = {"❌","🔍","📤","📩"};  

void log_printf(print_type type, const char *format, ...) {

    time_t rawtime;
    struct tm *timeinfo;
    char buffer[9];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    printf("[%s] %s ", buffer, print_type_str[type]);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void send_message(int sock, const char *message) {
    if (write(sock, message, strlen(message)) < 0) {
        log_printf(PRINT_ERROR, "Erreur lors de l'envoi du message\n");
    } else {
        log_printf(PRINT_SEND, "Message envoyé: %s", message);
    }
}
#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <signal.h>



#define BUFFER_SIZE_TINY 64
#define BUFFER_SIZE_SMALL 256 
#define BUFFER_SIZE_MEDIUM 512
#define BUFFER_SIZE 1024
#define BUFFER_SIZE_LARGE 2048

#define NOURRITURE "nourriture"
#define LINEMATE "linemate"
#define DERAUMERE "deraumere"
#define SIBUR "sibur"
#define MENDIANE "mendiane"
#define PHIRAS "phiras"
#define THYSTAME "thystame"
#define JOUEUR "joueur"

typedef enum    print_type {
    PRINT_ERROR,
    PRINT_INFORMATION,
    PRINT_SEND,
    PRINT_RECEIVE
}               print_type;

void            log_printf(print_type type, const char *format, ...);
void            send_message(int sock, const char *message);

#endif 
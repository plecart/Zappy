#include "../includes/tools.h"

void log_printf(const char *format, ...) {

    time_t rawtime;
    struct tm *timeinfo;
    char buffer[9];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    printf("[%s] ", buffer);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
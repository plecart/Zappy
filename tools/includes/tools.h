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

#define BUFFER_SIZE 1024

void        log_printf(const char *format, ...);

#endif 
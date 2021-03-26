/**
 * utils.c
 * Things many other files will need
 * 
 * Author: James Nicholson
 */

#include <errno.h>
#include <stdarg.h>
#include "uart.h"
#include "utils.h"



#define BUFFER_SIZE_FOR_FORMATTED_OUTPUT 4096

// Returns -1 if there was an error
long my_strtol(char *str)
{
    errno = 0;
    long bytes = strtol(str, NULL, 0);
    if (errno != 0)
    {
        return -1;
    }
    return bytes;
}

void myprintf(char *format, ...)
{
    char buffer[BUFFER_SIZE_FOR_FORMATTED_OUTPUT]; // holds the rendered string
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    uartPutsNL(UART2_BASE_PTR, buffer);
}
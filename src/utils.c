/**
 * utils.c
 * Things many other files will need
 * 
 * Author: James Nicholson
 */

#include "utils.h"
#include "uart.h"
#include "uartNL.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>

#define BUFFER_SIZE_FOR_FORMATTED_OUTPUT 8192

/**
 * Informed by this Stack Overflow post: https://stackoverflow.com/a/26083517
 */
long long my_strtoll(char *str)
{
    const char *nptr = str;                 /* string to read               */
    char *endptr = NULL;                        /* pointer to additional chars  */
    long long number = 0;                            /* variable holding return      */

    /* reset errno to 0 before call */
    errno = 0;

    /* call to strtol assigning return to number */
    number = strtoll(nptr, &endptr, 0);

    /* test return to number and errno values */
    // invalid  (no digits found, 0 returned)
    if (nptr == endptr) {
        return -1;
    }
    // invalid  (underflow occurred)
    else if (errno == ERANGE && number == LLONG_MIN) {
        return -1;
    }
    // invalid  (overflow occurred)
    else if (errno == ERANGE && number == LLONG_MAX) {
        return -1;
    }
    // invalid  (base contains unsupported value)
    else if (errno == EINVAL) { /* not in all c99 implementations - gcc OK */
        return -1;
    }
    // invalid  (unspecified error occurred)
    else if (errno != 0 && number == 0) {
        return -1;
    }
    // valid  (and represents all characters read)
    else if (errno == 0 && nptr && !*endptr) {
        return number;
    }
    // valid  (but additional characters remain)
    else if (errno == 0 && nptr && *endptr != 0) {
        return -1;
    }
    return -1;
}

int myprintf(char *format, ...)
{
    char buffer[BUFFER_SIZE_FOR_FORMATTED_OUTPUT]; // holds the rendered string
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    if (length < 0) {
        return E_MYPRINTF;
    }
    va_end(args);
    uartPutsNL(UART2_BASE_PTR, buffer);
    return E_SUCCESS;
}

int char_wash(char *dirty_chars, int num_chars, char *clean_chars)
{
    int chars_written = 0;
    for (int i=0; i<num_chars; i++) {
        if (dirty_chars[i] < 32 || dirty_chars[i] > 126)
        {
            // non-printing character, convert to hex and add to clean_chars
            char s[5];
            int x = 4660;
            sprintf(s, "%0x.2x", x);
            for (int j=0; j<4; j++) {
                clean_chars[chars_written] = s[j];
                chars_written++;
            }

        }
        // char is printable, add to clean_chars
        clean_chars[chars_written] = dirty_chars[i];
        chars_written++;
    }
    // null terminator
    clean_chars[chars_written] = 0;
    return E_SUCCESS;
}

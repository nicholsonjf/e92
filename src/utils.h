/**
 * utils.h
 * Things many other files will need
 * 
 * Author: James Nicholson
 */

#ifndef _MYUTIL_H
#define _MYUTIL_H


// Helper function prototypes.
long long my_strtoll(char *str);

int myprintf(char *format, ...);

int char_wash(char *dirty_chars, int num_chars, char *clean_chars);

// Shell error types.
enum error_t
{
    E_SUCCESS, // 0
    E_NOT_ENOUGH_ARGS,
    E_TOO_MANY_ARGS,
    E_CMD_NOT_FND,
    E_ARG_TYPE, // 4
    E_ADDR_NOT_ALLOCATED,
    E_WRONG_PID,
    E_MALLOC,
    E_STRTOL,
    E_BRANGE_EX, // 9
    E_ADDR_SPC,
    E_NO_MICRO_SD,
    E_FILE_STRUCT_MOUNTED,
    E_FILE_STRUCT_NOT_MOUNTED,
    E_FILE_NAME_INVALID, // 14
    E_FILE_NOT_IN_CWD,
    E_FILE_IS_DIRECTORY,
    E_FILE_EXISTS,
    E_FILE_OPEN,
    E_FILE_DELETE,
    E_FILE_NAME_TOO_LONG, // 20
    E_CHR_INVALID_8_3,
    E_NO_FREE_CLUSTER,
    E_MAX_STREAMS,
    E_DEVICE_PATH,
    E_GENERIC,
    E_NOT_SUPPORTED,
    E_MYPRINTF,
    E_LS,
    E_EXIT_DEVICES,
    E_EOF,
    E_READ_LIMIT,
    E_WRITE_LIMIT,
    E_FILE_CLOSED,
    E_COUNT // E_COUNT must be last to calculate the total number of error types
};

#endif /* ifndef _MYUTIL_H */

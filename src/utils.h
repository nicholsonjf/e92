/**
 * utils.h
 * Things many other files will need
 * 
 * Author: James Nicholson
 */

#ifndef _MYUTIL_H
#define _MYUTIL_H

// Helper function prototypes.
long my_strtol(char *str);
void myprintf(char *format, ...);

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
    E_STRTOUL,
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
    E_CHR_INVALID_8_3, // 20
    E_NO_FREE_CLUSTER,
    E_INIT_STREAM,
    E_DEVICE_PATH,
    E_GENERIC,
    E_COUNT
};

#endif /* ifndef _MYUTIL_H */
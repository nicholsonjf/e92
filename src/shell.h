#ifndef _MYSHELL_H
#define _MYSHELL_H

// Shell command function prototypes.
int cmd_date(int argc, char *argv[]);
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
int cmd_clockdate(int argc, char *argv[]);
int cmd_malloc(int argc, char *argv[]);
int cmd_free(int argc, char *argv[]);
int cmd_memory_map(int argc, char *argv[]);
int cmd_memset(int argc, char *argv[]);
int cmd_memchk(int argc, char *argv[]);

// Shell helper function prototypes.
long my_strtol(char *str);
void myprintf(char*format, ...);

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
    E_CHR_INVALID_8_3,
    E_NO_FREE_CLUSTER, // 19
    E_INIT_STREAM,
    E_COUNT
};

#endif /* ifndef _MYSHELL_H */
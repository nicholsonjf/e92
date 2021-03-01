// Shell command function prototypes.
int cmd_date(int argc, char *argv[]);
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
int cmd_clockdate(int argc, char *argv[]);
int cmd_malloc(int argc, char *argv[]);
int cmd_free(int argc, char *argv[]);
int cmd_memory_map(int argc, char *argv[]);

// Shell helper function prototypes.
unsigned long my_strtoul(char *str);

// Shell error types.
enum error_t
{
    E_SUCCESS,
    E_NOT_ENOUGH_ARGS,
    E_TOO_MANY_ARGS,
    E_CMD_NOT_FND,
    E_ARG_TYPE,
    E_ADDR_NOT_ALLOCATED,
    E_WRONG_PID,
    E_MALLOC,
    E_STRTOUL,
    E_COUNT
};
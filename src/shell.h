int strtob(char *str);

enum error_t
{
    E_SUCCESS = 0,
    E_NOT_ENOUGH_ARGS = 1,
    E_TOO_MANY_ARGS = 2,
    E_CMD_NOT_FND = 3,
    E_ARG_TYPE = 4,
    E_ADDR_NOT_ALLOCATED = 5,
    E_WRONG_PID = 6,
    E_MALLOC = 7
};
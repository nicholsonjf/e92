#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// Error code setup informed by https://stackoverflow.com/questions/6286874/c-naming-suggestion-for-error-code-enums
enum error_t
{
    E_SUCCESS = 0,
    E_NOT_ENOUGH_ARGS = 1,
    E_TOO_MANY_ARGS = 2,
    E_CMD_NOT_FND = 3,
    E_ARG_TYPE = 4
};

//typedef enum error_t cmd_err;

struct error_d
{
    int code;
    char *message;
} error_ds[] = {
    {E_SUCCESS, "No error"},
    {E_NOT_ENOUGH_ARGS, "Not enough arguments provided"},
    {E_TOO_MANY_ARGS, "Too many arguments provided"},
    {E_CMD_NOT_FND, "Please enter a valid command"},
    {E_ARG_TYPE, "Wrong argument type. Run the 'help' command for information about command arguments"}
    };

int print_err (int error_c)
{
    for (int i = 0; i <= 4; i++)
    {
        if (error_c == error_ds[i].code)
        {
            printf("ERROR: %s\n", error_ds[i].message);
        }
    }
    return 0;
}

struct date
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int microsecond;
};

struct monthMap
{
    int month_i;
    char *month_n;
} months[] = {
    {1, "January"},
    {2, "February"},
    {3, "March"},
    {4, "April"},
    {5, "May"},
    {6, "June"},
    {7, "July"},
    {8, "August"},
    {9, "September"},
    {10, "October"},
    {11, "November"},
    {12, "December"},
};

// Algorithm informed by http://howardhinnant.github.io/date_algorithms.html#civil_from_days
struct date calc_date(time_t tv_sec, suseconds_t tv_usec)
{
    struct date mydate;
    int ep_days = tv_sec / 86400;
    // Adjust epic days by number of days from 1970, 01, 01 to 0000, 03, 01
    int ep_days_adj = ep_days + 719468;
    // Calculate the era (an era is 400 years, there are 146097 days in 400 years)
    int era = ep_days_adj / 146097;
    // Days remaining after calculating the era
    int day_of_era = ep_days_adj % 146097;

    // Remaining number of seconds after days are divided out
    int ep_tod_secs = tv_sec % 86400;
    // 1460 is number of days in 4 years
    // 36524 is number of days in 100 years
    // 146096 is number of days in 400 years
    int year_of_era = ((day_of_era - (day_of_era / 1460) + (day_of_era / 36524) - (day_of_era / 146096)) / 365);
    int year = year_of_era + era * 400;
    int doy = day_of_era - (365 * year_of_era + (year_of_era / 4) - (year_of_era / 100));
    // Month prime
    int mp = (5 * doy + 2) / 153;
    mydate.month = mp + (mp < 10 ? 3 : -9);
    mydate.year = year + (mydate.month <= 2);
    mydate.day = doy - (153 * mp + 2) / 5 + 1;
    mydate.hour = ep_tod_secs / 3600;
    mydate.minute = (ep_tod_secs % 3600) / 60;
    mydate.second = (ep_tod_secs % 3600) % 60;
    mydate.microsecond = tv_usec;
    return mydate;
}

// Shell command function prototypes.
int cmd_date(int argc, char *argv[]);
int cmd_echo(int argc, char *argv[]);
int cmd_exit(int argc, char *argv[]);
int cmd_help(int argc, char *argv[]);
int cmd_clockdate(int argc, char *argv[]);

struct commandEntry
{
    char *name;
    int (*functionp)(int argc, char *argv[]);
} commands[] = {{"date", cmd_date},
                {"echo", cmd_echo},
                {"exit", cmd_exit},
                {"help", cmd_help},
                {"clockdate", cmd_clockdate}};

typedef int (*cmd_pntr)(int argc, char *argv[]);
cmd_pntr find_cmd(char *arg)
{
    for (int i = 0; i < 5; i++)
    {
        if (strcmp(arg, commands[i].name) == 0)
        {
            return commands[i].functionp;
        }
    }
    return NULL;
}

char *monthName (int month_i) {
    for (int i = 0; i < 12; i++)
    {
        if (months[i].month_i == month_i)
        {
            return months[i].month_n;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    while (1) {
        char linebuf[256];
        int index = 0;
        printf("$ ");
        while (1) {
            char c = fgetc(stdin);
            // Stop reading characters if we reach a newline.
            if (c == '\n') {
                linebuf[index] = 0;
                index++;
                break;
            }
            // Set whitespace to the null terminator.
            if (c == ' ' || c == '\t') {
                linebuf[index] = 0;
            }
            else {
                linebuf[index] = c;
            }
            index++;
        }
        // Previous character type.
        // 0 = whitespace, 1 = character.
        char pctype = 0;
        // Argument count.
        int argct = 0;
        // Iterate over characters in the line buffer and set argct, argval.
        for (int i=0; i<index; i++) {
            // If previous character is a space.
            if (pctype == 0) {
                // If current character is also a space.
                if (linebuf[i] == 0) {
                    continue;
                }
                // If current character is a character.
                else {
                    // Increment arg count, set arg pointer, set prev char type to character.
                    argct++;
                    pctype = 1;
                }
            }
            // If previous character is a character.
            else {
                // If current character is a space.
                if (linebuf[i] == 0) {
                    // Set prev char type to space.
                    pctype = 0;
                }
            }
        }
        // Initialize argument start indexes array.
        int arglocs[argct];
        // Initialize argument lengths array.
        int arglens[argct];
        int arglocct = 0;
        // Re-initialize previous character type to 0.
        pctype = 0;
        for (int i = 0; i < index; i++)
        {
            //printf("%d ", arglocct);
            // If previous character is a space.
            if (pctype == 0)
            {
                // If current character is also a space.
                if (linebuf[i] == 0)
                {
                    continue;
                }
                // If current character is a character.
                else
                {
                    // Set argument start index.
                    arglocs[arglocct] = i;
                    pctype = 1;
                }
            }
            // If previous character is a character.
            else
            {
                // If current character is a space.
                if (linebuf[i] == 0 || linebuf[i] == '\n')
                {
                    // Set argument length.
                    arglens[arglocct] = i - arglocs[arglocct];
                    // printf("Arglen/index: %d, %d\n", arglens[arglocct], i);
                    arglocct++;
                    // Set prev char type to space.
                    pctype = 0;
                }
            }
        }
        // Allocate space for argval
        char **argval = malloc((argct+1) * sizeof(char *));
        for (int i = 0; i < argct; i++)
        {
            // Assign argvals
            argval[i] = malloc((arglens[i] + 1) * sizeof(char));
            strncpy(argval[i], &linebuf[arglocs[i]], arglens[i]);
        }
        argval[argct] = NULL;
        cmd_pntr shell_cmd = find_cmd( argval[0] );
        if ( shell_cmd == NULL ) {
            print_err(E_CMD_NOT_FND);
        }
        else {
            // Only pass the arguments, not the shell command.
            int cmd_c = shell_cmd(argct - 1, &argval[1]);
            // If the command returns a non-zero error code, print the error message.
            if (cmd_c > 0) {
                print_err(cmd_c);
            }
        }
        // Free argval[i] and argval
        int i = 0;
        while (i < argct)
        {
            free(argval[i]);
            ++i;
        }
        free(argval);
    }
}

int cmd_date(int argc, char *argv[]) {
    struct timeval mytime;
    if (argc == 0)
    {
        gettimeofday(&mytime, NULL);
    }
    else if ( argc > 1 ) {
        return E_TOO_MANY_ARGS;
    }
    else {
        long result = 0;
        int len = strlen(argv[0]);
        for (int i = 0; i < len; i++)
        {
            result = result * 10 + (argv[0][i] - '0');
        }
        mytime.tv_sec = result;
        mytime.tv_usec = 0;
    }
    struct date mydate = calc_date(mytime.tv_sec, mytime.tv_usec);
    // Format: "January 23, 2014 15:57:07.123456"
    printf("%s %02d, %d %d:%02d:%02d.%d\n", monthName(mydate.month), mydate.day, mydate.year, mydate.hour, mydate.minute, mydate.second, mydate.microsecond);
    return E_SUCCESS;
}

int cmd_echo(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) {
        // print args.
        printf("%s", argv[i]);
        // Here, i is starting at zero and argc is not zero-indexed
        if (i < (argc-1)) {
            printf("%c", ' ');
        }
        else {
            printf("%c", '\n');
        }
    }
    return E_SUCCESS;
}

int cmd_exit(int argc, char *argv[])
{
    if (argc > 0)
    {
        return E_TOO_MANY_ARGS;
    }
    exit(E_SUCCESS);
}

int cmd_help(int argc, char *argv[])
{
    if (argc > 0)
    {
        return E_TOO_MANY_ARGS;
    }
    char *my_string = "Available Commands:\n"
                      "\n"
                      "     exit -- exit from the shell(i.e., cause the shell to terminate).\n"
                      "\n"
                      "     echo -- output each of the arguments to stdout.\n"
                      "\n"
                      "     help -- output to stdout a brief description of the commands accepted by the shell\n"
                      "\n"
                      "     date -- outputs to stdout the current date and time in the format: January 23, 2014 15:57:07.123456.\n"
                      "\n"
                      "clockdate -- takes a single positive integral number as its required argument.\n"
                      "             This number represents the number of seconds since the Unix Epoch. This provided\n"
                      "             Epoch time will be printed to stdout in the same format described in the date command.\n";
    printf("%s", my_string);
    return E_SUCCESS;
}
int cmd_clockdate(int argc, char *argv[])
{
    if (argc == 0) {
        return E_NOT_ENOUGH_ARGS;
    }
    else if (argc > 1) {
        return E_TOO_MANY_ARGS;
    }
    for (int i=0; i<strlen(argv[0]); i++) {
        if (argv[0][i] < '0' || argv[0][i] > '9' )
        {
            return E_ARG_TYPE;
        }
    }
    cmd_date(1, argv);
    return E_SUCCESS;
}
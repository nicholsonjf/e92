#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

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

struct date calc_date(time_t tv_sec, suseconds_t tv_usec){
    struct date mydate;
    // Calculate year
    // While tv_sec > num of seconds in year, subtract number of seconds in year (checking for leap each time)
    // Eventually num of seconds will drop below the num of seconds in current year
    // To compute month, write a function that takes year, month and gives you number of seconds in month (i.e. if leap year feb is different)
    int ep_days = tv_sec / 86400;
    int ep_days_adj = ep_days - 58;
    int ep_days_r = tv_sec % 86400;

    mydate.year = 1970;
    mydate.month = 0;
    mydate.hour = ep_days_r / 3600;
    mydate.minute = (ep_days_r % 3600) / 60;
    mydate.second = (ep_days_r % 3600) % 60;
    mydate.microsecond = tv_usec;
    return mydate;
}

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

int main(int argc, char** argv) {
    while (1) {
        // Is a line guaranteed to be < 256 bytes?
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
        // Allocate and assign argvals
        char **argval = malloc((argct+1) * sizeof(char *));
        // TODO do we need to add a null terminator to the end of argval?
        for (int i = 0; i < argct; i++)
        {
            argval[i] = malloc((arglens[i] + 1) * sizeof(char));
            strncpy(argval[i], &linebuf[arglocs[i]], arglens[i]);
        }
        argval[argct] = NULL;
        cmd_pntr shell_cmd = find_cmd( argval[0] );
        if ( shell_cmd == NULL ) {
            printf( "Please enter a valid command\n" );
        }
        else {
            // Only pass the arguments, not the shell command.
            shell_cmd( argct-1, &argval[1] );
        }
    }
}

int cmd_date(int argc, char *argv[]) {
    struct timeval mytime;
    gettimeofday(&mytime, NULL);
    struct date mydate = calc_date(mytime.tv_sec, mytime.tv_usec);
    printf("%d %d %d %d\n", mydate.hour, mydate.minute, mydate.second, mydate.microsecond);
    return 0;
}
int cmd_echo(int argc, char *argv[]){
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
    return 0;
}
int cmd_exit(int argc, char *argv[]){
    exit(0);
}
int cmd_help(int argc, char *argv[]){
    return 0;
}
int cmd_clockdate(int argc, char *argv[]){

    return 0;
}

// TODO Using the debugger
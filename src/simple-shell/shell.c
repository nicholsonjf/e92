#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                    printf("Arglen/index: %d, %d\n", arglens[arglocct], i);
                    arglocct++;
                    // Set prev char type to space.
                    pctype = 0;
                }
            }
        }
        char **argval = malloc((argct+1) * sizeof(char *));
        for (int i = 0; i < argct; i++)
        {
            argval[i] = malloc((arglens[i] + 1) * sizeof(char));
            strncpy(argval[i], &linebuf[arglocs[i]], arglens[i]);
            // print args.
            printf("%s ", argval[i]);
        }
        cmd_pntr shell_cmd = find_cmd( argval[0] );
        if ( shell_cmd == NULL ) {
            printf( "Please enter a valid command\n" );
        }
        else {
            shell_cmd( argct, argval );
        }
    }
}

int cmd_date(int argc, char *argv[]) {
    return 0;
}
int cmd_echo(int argc, char *argv[]){
    return 0;
}
int cmd_exit(int argc, char *argv[]){
    return 0;
}
int cmd_help(int argc, char *argv[]){
    return 0;
}
int cmd_clockdate(int argc, char *argv[]){
    return 0;
}
#include <stdio.h>


int main(int argc, char** argv) {
    // printf("%d %s\n", argc, argv[1]);
    while (1) {
        // Is a line guaranteed to be < 256 bytes?
        char linebuf[256];
        int index = 0;
        printf("$");
        while (1) {
            char c = fgetc(stdin);
            // Stop reading characters if we reach a newline.
            if (c == '\n') {
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
        // Example for how to set argv.
        // argv[0] = &linebuf[0]
        // argv[1] = &linebuf[10]

        // Previous character type.
        // 0 = whitespace, 1 = character.
        char pctype = 0;
        // Argument count.
        int argct = 0;
        // Iterate over characters in the line buffer and set argct, argval.
        for (int i=0; i<index; i++) {
            printf("%d\n", i);
            // If previous character is a space.
            if (pctype == 0) {
                // If current character is also a space.
                if (linebuf[i] == 0) {
                    continue;
                }
                // If current character is a character.
                else {
                    printf("%s\n", "Character after space");
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
        // Assign arguement start indexes.
        int *arglocs[argct];
        int *arglens[argct];
        int arglocct = 0;
        // Re-initialize previous character type to 0.
        pctype = 0;
        for (int i = 0; i < index; i++)
        {
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
                    arglocs[arglocct] = &i;
                    pctype = 1;
                }
            }
            // If previous character is a character.
            else
            {
                // If current character is a space.
                if (linebuf[i] == 0)
                {
                    // Set argument length.
                    arglens[arglocct] = i - *arglocs[arglocct];
                    arglocct++;
                    // Set prev char type to space.
                    pctype = 0;
                }
            }
        }
        char **argval;
        argval = *malloc((argct + 1) * sizeof(char *));
        for (int i = 0; i < argct; i++)
        {
            argval[i] = malloc((*arglens[i] + 1) * sizeof(char));
            strncpy(argval[i], linebuf[*arglocs[i]], arglens[i]);
            // argval[argct] = &linebuf[i];
        }
        printf("Number of args: %d\n", argct);
    }
}

#include <stdio.h>


int main(int argc, char** argv) {
    printf("%d %s\n", argc, argv[1]);
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
        char** argval;
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
                    // argval[argct] = &linebuf[i];
                    pctype = 1;
                }
            }
            else {
                // If previous character is a character.
                if (linebuf[i] == 0) {
                    // Set prev char type to space.
                    pctype = 0;
                }
            }
        }
        printf("Number of args: %d\n", argct);
    }
}

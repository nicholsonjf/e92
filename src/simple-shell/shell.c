#include <stdio.h>

char shell(void) {
    char c = fgetc(stdin);
    return c;
}

int main(int argc, char** argv) {
    printf("%d %s\n", argc, argv[1]);
    while (1) {
        char linebuf[256];
        int index = 0;
        printf("$");
        while (1) {
            char c = fgetc(stdin);
            if (c == '\n') {
                break;
            }
            if (c == ' ' || c == '\t') {
                linebuf[index] = 0;
            }
            else {
                linebuf[index] = c;
            }
            index++;
        }
        // argv[0] = &linebuf[0]
        // argv[1] = &linebuf[10]
        // 0 = whitespace, 1 = word
        char pctype = 0;
        int argct = 0;
        for (int i=0; i<index; i++) {
            if (pctype == 0) {
                if (linebuf[i] == 0) {
                    continue;
                }
                else {
                    argct++;
                    pctype = 1;
                }
            }
            else {
                if (linebuf[i] == 0) {
                    pctype = 0;
                }
            }
        }

    }
}

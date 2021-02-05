#include <stdio.h>

char shell(void) {
    char c = fgetc(stdin);
    return c;
}

int main(int argc, char** argv) {
    printf("$");
    while (1) {
        char c = fgetc(stdin);
        if (c == '\n') {
            break;
        }
    }
}

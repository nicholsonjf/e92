#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    char* word;
    struct Node *next;
};

typedef struct List {
    struct Node *head;
    struct Node *tail;
};

int build_list(struct List* list, char* filename) {
    FILE *inf = fopen(filename, "r");
    if (inf == NULL)
    {
        printf("%s\n", "Failed to open file, or it doesn't exist.");
        exit(1);
    }
    list->head = NULL;
    list->tail = NULL;

    char* word = malloc(1);
    int my_llist = readWord(inf, word);
    while (my_llist == NO_ERR)
    {
        my_llist = add_word_to_list(&list, &word);
        if (my_llist == NO_ERR)
        {
            word = malloc(1);
            my_llist = read_word(file, &word);
        }
    }
    free(word);
    return my_llist;
}

int readWord(FILE* file, char** word)
{
    int word_size = 1;
    int word_count = 0;
    int c = fgetc(file);
    while (c != ' ' && c != EOF)
    {
        if (word_count == word_size)
        {
            word_size += 1;
            *word = realloc(*word, word_size);
        }

        (*word)[word_count++] = c;
        c = fgetc(file);
    }

    *word = realloc(word, word_size + 1);
    // Set the null character at the end of the character array
    (*word)[word_size] = '\0';

    if (c == EOF){
        return c;
    }
    return(0);
}


int add_word_to_list(struct List* list, char* word)
{
    int result = 0;
    struct Node* newNode = malloc (sizeof(struct Node));
    if (newNode == NULL)
    {
        printf("%s\n", "Out of memory.");
        exit(1);
    }
    else
    {
        newNode->next = NULL;
        newNode->word = word;
        if (list->head == NULL)
        {
            list->head = newNode;
        }
        else
        {
            list->tail->next = newNode;
        }
        list->tail = newNode;
    }
    return result;
}

int main( int argc, char *argv[] ) {
    char inf_name[30];
    if ( argc == 2 ) {
        scanf( "%s", inf_name );
    } else {
        printf( "%s\n", "Please provide one argument with the filename to read." );
    }

    List wordList;
    int wordll = build_list(&wordList, inf_name);
    return 0;
}
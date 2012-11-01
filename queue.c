#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 80
#define WRAP_SIZE 65

size_t total_chars = 0,
       total_words = 0,
       total_lines = 0;

struct {
    char s[BUFF_SIZE];
    size_t i;
} buf;

void read_file(FILE *h);

int main(int argc, char **argv) {
    int i;
    if (argc < 2)
        read_file(stdin);
    for (i = 1; i < argc; i++) {
        FILE *in = fopen(argv[i], "r");
        if (in) {
            read_file(in);
            fclose(in);
        }
    }
    return 0;
}

void read_file(FILE *h) {
    char c;
    while ((c = getc(h)) != EOF) {
        if (c == '\n') {
            buf.s[buf.i] = '\0';
            puts(buf.s);
            buf.i = 0;
        } else {
            buf.s[buf.i++] = c;
        }
    }
}



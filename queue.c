#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 80
#define WRAP_SIZE 65

size_t total_chars = 0,
       total_words = 0,
       total_lines = 0;

struct {
    char s[BUFF_SIZE];
} buf;

int main(int argc, char **argv) {
    puts(gets(buf.s));
    return 0;
}



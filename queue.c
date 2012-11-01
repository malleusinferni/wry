#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define bool char
#define TRUE 1
#define FALSE 0

void tail_test(int n, int argc, char **argv);
void line_test(void);
void reset_test(void);
void wbeg_test(void);

int main(int argc, char **argv) {
    int i;
    init_buf();
    wbeg_test();
    return 0;
}

void tail_test(int n, int argc, char **argv) {
    int i;
    FILE *in;
    if (argc < 2)
        read_file(stdin);
    for (i = 1; i < argc; i++) {
        if ((in = fopen(argv[i], "r"))) {
            read_file(in);
            fclose(in);
        }
    }
    drop_until(n);
    print_queue();
}

void line_test() {
    int i, j, x;
    char LINE_TEST[] = "This is only    a test.",
         LINE_BUFF[BUFF_SIZE];
    for (i = 0; i < strlen(LINE_TEST); i++) {
        append_buf(LINE_TEST[i]);
        x = 0;
        for (j = 0; j < buf.i; j++) {
            if (j == buf.wbreak && j == buf.wbeg) {
                LINE_BUFF[j + x++] = '|';
            } else if (j == buf.wbreak) {
                LINE_BUFF[j + x++] = '[';
            } else if (j == buf.wbeg) {
                LINE_BUFF[j + x++] = ']';
            }
            LINE_BUFF[j + x] = buf.s[j];
        }
        LINE_BUFF[j + x] = '\0';
        printf("%s\n", LINE_BUFF);
    }
    printf("%lu words\n", buf.wc);
}

void reset_test() {
    char LINE_BUFF[BUFF_SIZE];
    reset_buf("Some other text");
    strncpy(LINE_BUFF, buf.s, buf.i);
    LINE_BUFF[buf.i] = '\0';
    printf("RESET (%lu words): %s\n", buf.wc, LINE_BUFF);
}

void wbeg_test() {
    reset_buf("THIS TEST IS A SUCCESS");
    buf.s[buf.i] = '\0';
    printf("%s\n", buf.s + buf.wbeg);
}


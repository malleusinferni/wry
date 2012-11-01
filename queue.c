#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define bool char
#define TRUE 1
#define FALSE 0
#define BUFF_SIZE 80
#define WRAP_SIZE 65

typedef struct line_t {
    struct line_t *next;
    char s[0];
} line_t;

struct {
    char s[BUFF_SIZE];
    size_t i, wbreak;
    bool inword;
    line_t *top, *bot;
} buf;

struct {
    size_t chars, words, lines;
} total;

void init_buf(void);
void print_queue(void);
void drop_until(int count);
void shift_line(void);
void push_line(char *s);
void insert_ch(char c);
void break_at(size_t i);
void read_file(FILE *h);
void tail_test(int n, int argc, char **argv);
void line_test(void);

int main(int argc, char **argv) {
    int i;
    init_buf();
    line_test();
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
    int i, len;
    char LINE_TEST[] = "This is only a test.",
         LINE_BUFF[BUFF_SIZE];
    for (i = 0; i < strlen(LINE_TEST); i++) {
        insert_ch(LINE_TEST[i]);
        len = buf.wbreak;
        strncpy(LINE_BUFF, buf.s, len);
        LINE_BUFF[len] = '\0';
        printf("%s|", LINE_BUFF);
        len = buf.i - len;
        strncpy(LINE_BUFF, buf.s + buf.wbreak, len);
        LINE_BUFF[len] = '\0';
        printf("%s\n", LINE_BUFF);
    }
}

void init_buf() {
    buf.i = 0;
    buf.inword = FALSE;
    buf.top = buf.bot = NULL;
    total.chars = total.words = total.lines = 0;
}

void push_line(char *s) {
    size_t len = strlen(s) + 1;
    line_t *nl = malloc(sizeof(line_t) + len);
    strncpy(nl->s, s, len);
    nl->next = NULL;
    if (buf.top) {
        buf.bot->next = nl;
        buf.bot = nl;
    } else {
        buf.top = nl;
        buf.bot = nl;
    }
}

void shift_line() {
    line_t *temp = buf.top;
    buf.top = temp->next;
    free(temp);
}

void print_queue() {
    line_t *line = buf.top;
    while (line) {
        puts(line->s);
        line = line->next;
    }
}

void drop_until(int count) {
    int qsize = 0;
    line_t *line = buf.top;
    while (line) {
        line = line->next;
        qsize++;
    }

    while (qsize > count) {
        shift_line();
        qsize--;
    }
}

void break_at(size_t i) {
    buf.s[i] = '\0';
    push_line(buf.s);
    buf.i = 0;
}

void insert_ch(char c) {
    if (c == '\n') {
        break_at(buf.i);
    } else {
        if (isspace(c) && buf.inword) {
            buf.wbreak = buf.i;
            buf.inword = FALSE;
        } else if (!buf.inword) {
            buf.inword = TRUE;
        }
        buf.s[buf.i] = c;
        buf.i++;
    }
}

void read_file(FILE *h) {
    char c;
    while ((c = getc(h)) != EOF) {
        insert_ch(c);
    }
}



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
    size_t i, wc, wbreak, wbeg;
    bool inword;
    line_t *top, *bot;
} buf;

struct {
    size_t chars, words, lines;
} total;

void init_buf(void);
void reset_buf(char *s);
void append_buf(char c);
void print_queue(void);
void drop_until(int count);
void shift_line(void);
void push_line(char *s);
void insert_ch(char c);
void break_at(size_t i);
void read_file(FILE *h);
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

void init_buf() {
    reset_buf("");
    buf.top = buf.bot = NULL;
    total.chars = total.words = total.lines = 0;
}

void reset_buf(char *s) {
    buf.i = buf.wc = buf.wbreak = buf.wbeg = 0;
    buf.inword = FALSE;
    if (s != NULL) {
        int i, len = strlen(s);
        for (i = 0; i < len; i++)
            insert_ch(s[i]);
    }
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
    if (isspace(c)) {
        if (buf.inword) {
            buf.wbreak = buf.i;
            buf.inword = FALSE;
        }
        buf.wbeg = buf.i + 1;
    } else if (!buf.inword) {
        buf.inword = TRUE;
        buf.wc++;
    }
    buf.s[buf.i] = c;
    buf.i++;
}

void append_buf(char c) {
    if (c == '\n') {
        break_at(buf.i);
    } else {
        insert_ch(c);
        if (buf.i > WRAP_SIZE) {
            char save[BUFF_SIZE] = "";
            if (buf.inword) {
                // TODO Handle words too long to wrap
                strncpy(save, buf.s + buf.wbeg, buf.i - buf.wbeg);
            }
            break_at(buf.wbreak);
            reset_buf(save);
        }
    }
}

void read_file(FILE *h) {
    char c;
    while ((c = getc(h)) != EOF) {
        append_buf(c);
    }
}



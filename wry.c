#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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
    FILE *out;
    line_t *top, *bot;
} buf;

struct {
    size_t chars, words, lines;
} total;

char minibuffer[BUFF_SIZE],
     file_name[BUFF_SIZE];

bool needs_redisplay = TRUE;

void read_file(char *name);
void minibufmsg(char *s);
void printbuf(const char * restrict format, ...);

void init_buf(void);
void reset_buf(char *s);
void append_buf(char c);
void insert_ch(char c);
void break_at(size_t i);

void push_line(char *s);
void shift_line(void);
void drop_until(int count);
void print_queue(void);

int main(int argc, char **argv) {
    init_buf();
    initscr();
    cbreak();
    noecho();
    if (argc > 1) read_file(argv[1]);

    while (TRUE) {
        int ch;
        // Redisplay
        drop_until(getmaxy(stdscr) / 2);
        print_queue();
        minibufmsg("Press any key to quit");
        refresh();

        // Wait for input
        if ((ch = getch())) {
            if (buf.i > 0)
                append_buf('\n');
            fclose(buf.out);
            endwin();
            exit(0);
        }
    }
    return 0;
}

void read_file(char *name) {
    FILE *h;
    char c;
    bool newfile = FALSE;
    if ((h = fopen(name, "r"))) {
        while ((c = getc(h)) != EOF)
            append_buf(c);
        fclose(h);
    } else if (errno == ENOENT) {
        newfile = TRUE;
    } else {
        printbuf("Error opening %s: %s", name, strerror(errno));
        return;
    }
    if ((h = fopen(name, "a"))) {
        buf.out = h;
        if (newfile)
            printbuf("\"%s\" [NEW FILE]", name);
        else
            printbuf("\"%s\"", name);
    }
}

void minibufmsg(char *s) {
    int cy, cx, y, x, rlen;
    char ruler[BUFF_SIZE];
    snprintf(ruler, BUFF_SIZE,
        " Ln %lu Wd %lu Ch %lu    ",
        total.lines + 1,
        total.words + buf.wc,
        total.chars + buf.i);
    rlen = strlen(ruler);
    getyx(stdscr, cy, cx);
    getmaxyx(stdscr, y, x);
    move(y - 1, 0);
    clrtoeol();
    attron(A_BOLD);
    mvprintw(y - 1, 0, "%s", s);
    if (strlen(s) < x - rlen)
        mvprintw(y - 1, x - rlen, "%s", ruler);
    attroff(A_BOLD);
    move(cy, cx);
}

void printbuf(const char * restrict format, ...) {
    va_list ap;
    va_start(ap, format);
    vsnprintf(minibuffer, BUFF_SIZE, format, ap);
    va_end(ap);
    minibufmsg(minibuffer);
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

void append_buf(char c) {
    if (c == '\n') {
        break_at(buf.i);
        reset_buf("");
    } else {
        insert_ch(c);
        if (buf.i > WRAP_SIZE) {
            char save[BUFF_SIZE] = "";
            if (buf.inword) {
                // TODO Handle words too long to wrap
                strncpy(save, buf.s + buf.wbeg, buf.i - buf.wbeg);
                buf.wc--;
            }
            break_at(buf.wbreak);
            reset_buf(save);
        }
    }
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

void break_at(size_t i) {
    buf.s[i] = '\0';
    total.words += buf.wc;
    total.chars += strlen(buf.s) + 1;
    total.lines++;
    push_line(buf.s);
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
    if (buf.out) {
        fputs(nl->s, buf.out);
        // fflush(buf.out);
    }
}

void shift_line() {
    line_t *temp = buf.top;
    buf.top = temp->next;
    free(temp);
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

void print_queue() {
    line_t *line = buf.top;
    while (line) {
        printw("%s\n", line->s);
        line = line->next;
    }
}


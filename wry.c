#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void read_file(FILE *h);
void minibufmsg(char *s);

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
    initscr();
    minibufmsg("Press any key to quit.");

    refresh();
    getch();
    endwin();
    return 0;
}

void minibufmsg(char *s) {
    int cy, cx, y, x, rlen;
    char ruler[BUFF_SIZE];
    snprintf(ruler, BUFF_SIZE,
        " %luln %luwd %luch    ",
        total.lines,
        total.words,
        total.chars);
    rlen = strlen(ruler);
    getyx(stdscr, cy, cx);
    getmaxyx(stdscr, y, x);
    attron(A_BOLD);
    mvprintw(y - 1, 0, "%s", s);
    if (strlen(s) < x - rlen)
        mvprintw(y - 1, x - rlen, "%s", ruler);
    attroff(A_BOLD);
    move(cy, cx);
}


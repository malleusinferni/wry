#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DOC_SIZE (2 << 15)
#define RULER_SIZE 80

struct {
    char *text;
    size_t char_count;
    size_t word_count;
    size_t line_count;
} document;

int g_lines, g_columns;
void minibufmsg(char *s);
void document_wc(void);

int main(int argc, char **argv) {
    document.text = malloc(DOC_SIZE);
    document.text[0] = '\0';
    document_wc();

    initscr();
    getmaxyx(stdscr, g_lines, g_columns);
    minibufmsg("Press any key to quit.");

    printw("%s\n", document.text);

    refresh();
    getch();
    endwin();
    return 0;
}

void minibufmsg(char *s) {
    int cy, cx, y, x, rlen;
    char ruler[RULER_SIZE];
    snprintf(ruler, RULER_SIZE,
        " %luln %luwd %luch    ",
        document.line_count,
        document.word_count,
        document.char_count);
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

void document_wc() {
    int i = 0, w = 0, l = 1, inword = FALSE;
    while (document.text[i]) {
        i++;
        if (document.text[i] == '\n') {
            l++;
        }

        if (isspace(document.text[i])) {
            inword = FALSE;
        } else if (!inword) {
            inword = TRUE;
            w++;
        }
    }
    document.char_count = i;
    document.word_count = w;
    document.line_count = l;
}


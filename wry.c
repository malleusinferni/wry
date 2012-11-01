#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define RULER_SIZE 80

void minibufmsg(char *s);

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
    char ruler[RULER_SIZE];
    snprintf(ruler, RULER_SIZE,
        " %luln %luwd %luch    ",
        0l, 0l, 0l);
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


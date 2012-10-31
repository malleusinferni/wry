#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define RULER_SIZE 80

char *document;

int g_lines, g_columns;
void minibufmsg(char *s);

int main(int argc, char **argv) {
    document = malloc(2 << 15);
    document[0] = '\0';

    initscr();
    getmaxyx(stdscr, g_lines, g_columns);
    minibufmsg("Press any key to quit.");
    printw("LINES=%d\nCOLS=%d\n", g_lines, g_columns);
    refresh();
    getch();
    endwin();
    return 0;
}

void minibufmsg(char *s) {
    int cy, cx, y, x, rlen;
    char ruler[RULER_SIZE];
    snprintf(ruler, RULER_SIZE,
        "%luch", strlen(document));
    rlen = strlen(ruler) + 1;
    getyx(stdscr, cy, cx);
    getmaxyx(stdscr, y, x);
    mvprintw(y - 1, 0, "%s", s);
    mvprintw(y - 1, x - rlen, "%s", ruler);
    move(cy, cx);
}


#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

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
    int cy, cx;
    int y, x;
    getyx(stdscr, cy, cx);
    getmaxyx(stdscr, y, x);
    mvprintw(y - 1, 0, "%s", s);
    move(cy, cx);
}


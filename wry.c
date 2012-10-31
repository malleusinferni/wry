#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

int g_lines, g_columns;

int main(int argc, char **argv) {
    initscr();
    getmaxyx(stdscr, g_lines, g_columns);
    printw("LINES=%d\nCOLS=%d\n", g_lines, g_columns);
    refresh();
    getch();
    endwin();
    return 0;
}


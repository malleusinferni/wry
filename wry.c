#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

int g_lines, g_columns;

int main(int argc, char **argv) {
    initscr();
    getmaxyx(stdscr, g_lines, g_columns);
    endwin();
    printf("LINES=%d\nCOLUMNS=%d\n", g_lines, g_columns);
    return 0;
}


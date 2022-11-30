/* Wry: A text editor for vampires






*/

/* 
 * Copyright (c) 2012, RSMW.net
 *
 * This software is under the WTF Public License.
 * Just do whatever the fuck you want with it.
 */

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define WRAP_SIZE 65

typedef struct line_t {
    struct line_t *next;
    char s[0];
} line_t;

struct {
    char s[BUFSIZ];
    size_t i, wc, wbreak, wbeg;
    bool inword;
    FILE *out;
    line_t *top, *bot;
} buf;

struct {
    size_t chars, words, lines;
} total;

char minibuffer[BUFSIZ],
     file_name[BUFSIZ];

bool needs_redisplay = TRUE;

int minibuf_attrs = 0;
void read_file(char *name);
void mbuf_display(void);
void mbuf_msg(char *s);
void mbuf_fmt(const char * format, ...);
void save_quit(void);
void save(void);
void quit(void);
void init_buf(void);
void reset_buf(char *s);
void append_buf(char c);
void del_buf(size_t i);
void insert_ch(char c);
void break_at(size_t i);

void push_line(char *s);
void shift_line(void);
void drop_until(int count);
void print_queue(void);

int main(int argc, char **argv) {

    //Initialize buffer
    init_buf();

    //Open file by args otherwise open default file
    if (argc > 1)
        strncpy(file_name, argv[1], BUFSIZ);
    else
        strcpy(file_name, "untitled.txt");

    // Read our....file,duh
    read_file(file_name);
    initscr();
    cbreak();
    noecho();


    if (has_colors()) {
        int white;
        start_color();
        if (COLORS > 255)
            white = 231;
        else if (COLORS > 87)
            white = 79;
        else
            white = 15;
        init_pair(1, white, 0);
        minibuf_attrs = COLOR_PAIR(1);
    } else {
        minibuf_attrs = A_BOLD;
    }


    //Main loop
    while (TRUE) {
        int ch;
        // Redisplay
        drop_until(getmaxy(stdscr) / 2);
        print_queue();
        mbuf_display();
        refresh();

        // Wait for input
        switch ((ch = getch())) {
        case EOF:
        case '\x04': // ^D
            save_quit();
        case '\x0F': // ^O
            save();
            break;
        case '\x12': // ^R
            quit();
        case '\x08': // ^H
            save();
            mbuf_msg("Saved");            
            break;
        case '\x7f': // DEL
            del_buf(buf.i - 1); break;
        case '\x17': // ^W delete line
            del_buf(buf.wbeg); break;
        case '\x15': // ^U
            reset_buf("");
            break;
        case '\x1b': // ESC
            mbuf_msg("Press CTRL+D to quit");
            break;
        case '\r':
        case '\n':
            append_buf('\n');
            break;
        default:
            mbuf_msg(file_name);
            if (ch > '\x1f')
                append_buf(ch);
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
        mbuf_fmt("Error opening %s: %s", name, strerror(errno));
        return;
    }
    if ((h = fopen(name, "a"))) {
        buf.out = h;
        mbuf_fmt(newfile ? "%s [NEW FILE]" : "%s", name);
    }
}

void mbuf_display() {
    int cy, cx, y, x, rlen;
    char ruler[BUFSIZ];
    //write to the buffer
    //ruler is our word line chars count at the 
    //bottom right of the screen
    snprintf(ruler, BUFSIZ,
        " Ln %lu Wd %lu Ch %lu    ",
        total.lines + 1,
        total.words + buf.wc,
        total.chars + buf.i);
    rlen = strlen(ruler);
    //get curses cursor and window coordinates
    getyx(stdscr, cy, cx);
    getmaxyx(stdscr, y, x);
    //move curses window cursor 
    move(y - 1, 0);
    //clear curses window
    clrtoeol();
    
    attron(minibuf_attrs);
    mvprintw(y - 1, 0, "%s", minibuffer);
    if (strlen(minibuffer) < x - rlen)
        mvprintw(y - 1, x - rlen, "%s", ruler);
    attroff(minibuf_attrs);
    move(cy, cx);
}

void mbuf_msg(char *s) {
    snprintf(minibuffer, BUFSIZ, "%s", s);
}

void mbuf_fmt(const char * format, ...) {
    va_list ap;
    va_start(ap, format);
    vsnprintf(minibuffer, BUFSIZ, format, ap);
    va_end(ap);
}

void save() {
    if (buf.i > 0)
        break_at(buf.i);
        reset_buf("");
}

void save_quit() {
    if (buf.i > 0)
        append_buf('\n');
    fclose(buf.out);
    endwin();
    exit(0);
}

void quit() {
    fclose(buf.out);
    endwin();
    exit(0);
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
            char save[BUFSIZ] = "";
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

void del_buf(size_t i) {
    char save[BUFSIZ];
    if (buf.i < 1) return;
    strncpy(save, buf.s, i);
    save[i] = '\0';
    reset_buf(save);
}

void insert_ch(char c) {
    if (isspace(c)) {
        if (buf.inword) {
            buf.wbreak = buf.i;
            buf.inword = FALSE;
        }
    } else if (!buf.inword) {
        buf.wbeg = buf.i;
        buf.inword = TRUE;
        buf.wc++;
    }
    buf.s[buf.i] = c;
    buf.i++;
}

// Update stats and call pushline to save to buffer
void break_at(size_t i) {
    buf.s[i] = '\0';
    total.words += buf.wc; //word count
    total.chars += strlen(buf.s) + 1; //character count
    total.lines++; //number of lines
    push_line(buf.s);
}

// write screen contents to buffer (file)
void push_line(char *s) {
    size_t len = strlen(s) + 1;
    //lines on screen
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
        fprintf(buf.out, "%s\n", nl->s);
        fflush(buf.out);
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
    char screenbuf[BUFSIZ];
    line_t *line = buf.top;
    move(0, 0);
    clrtobot();
    while (line) {
        printw("%s\n", line->s);
        line = line->next;
    }
    strncpy(screenbuf, buf.s, buf.i);
    screenbuf[buf.i] = '\0';
    printw("%s", screenbuf);
}


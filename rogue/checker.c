#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>
#include <menu.h>

int main() {
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    refresh();
    attron(A_BLINK | A_REVERSE);
    char s[10] = "ၜo";
    mvprintw(10, 10, "%s", s);
    refresh();
    getch();
    endwin();

    return 0;
}

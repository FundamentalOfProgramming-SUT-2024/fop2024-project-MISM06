#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>
#include <menu.h>

int main() {
    setlocale(LC_ALL,"");
    initscr();
    curs_set(0);
    noecho();
    start_color();
    use_default_colors();
    // set_colors();
    // init_elmnts();
    keypad(stdscr, true);
    cbreak();
    refresh();

    init_pair(57, 57, COLOR_BLACK);
    attron(A_REVERSE | COLOR_PAIR(57));
    for (int i = 0; i < 30; i++) {
        mvprintw(1, i, "o"); 
    }
    refresh();
    getch();
    endwin();

    return 0;
}

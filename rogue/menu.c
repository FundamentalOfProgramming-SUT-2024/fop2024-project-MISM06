#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<json-c/json.h>

#define center_line 20
#define center_col 50

void initial_page() {
    WINDOW *win = newwin(LINES, COLS, 0, 0);
    keypad(win, true);
    char *msg[] = {"ROGUE", "Pess any key to continue!"};
    wattron(win, A_BOLD | COLOR_PAIR(11) | A_REVERSE);
    mvwaddstr(win, LINES / 2, (COLS - strlen(msg[0])) / 2, msg[0]);
    wattroff(win, A_BOLD | COLOR_PAIR(11) | A_REVERSE);
    wattron(win, A_DIM | A_BLINK);
    mvwaddstr(win, LINES / 2 + 1, (COLS - strlen(msg[1])) / 2, msg[1]);
    wrefresh(win);
    getch();
    wclear(win);
    wrefresh(win);
    delwin(win);
}
void message_box (char *msg) { //you can use it to interact with player
    WINDOW *win = newwin(3, strlen(msg) + 2, 0, 0);
    keypad(win, true);
    wattron(win, A_BOLD | A_REVERSE | COLOR_PAIR(13));
    box(win, ' ', ' ');
    mvwaddstr(win, 1, 1, msg);
    wrefresh(win);
    getch();
    wclear(win);
    wrefresh(win);
    delwin(win);
}
int do_menu_stuff (int num_op, char **option, int *slcted) {
    int selected = *slcted;
    int starty = (LINES - center_line) / 2;
    int startx = (COLS - center_col) / 2;
    WINDOW *menu = newwin(center_line, center_col, starty, startx);
    keypad(menu, true);
    wattron(menu, A_DIM);
    box(menu, 0, 0);
    wattroff(menu, A_DIM);
    int ch;
    do {
        wattron(menu, COLOR_PAIR(13));
        for (int i = 0; i < num_op; i++) {
            if (i == selected) wattron(menu, A_REVERSE);
            mvwaddstr(menu, i + 1, 1, option[i]);
            if (i == selected) wattroff(menu, A_REVERSE);
        }
        wrefresh(menu);
        ch = wgetch(menu);
        switch(ch) {
            case KEY_UP :
                selected += num_op;
                --selected;
                selected %= num_op;
                break;
            case KEY_DOWN :
                ++selected;
                selected %= num_op;
                break;
            case 10 :
                break;
            default :
                message_box("Use up and down keys!");
        }
    } while(ch != 10);
    // wrefresh(menu);
    wclear(menu);
    delwin(menu);
    *slcted = selected;
    return selected;
}

int main() {

    initscr();
    curs_set(0);
    noecho();
    start_color();
    init_pair(11, COLOR_RED, COLOR_BLACK);
    init_pair(12, COLOR_BLUE, COLOR_BLACK);
    init_pair(13, COLOR_CYAN, COLOR_BLACK);
    keypad(stdscr, true);
    cbreak();
    refresh();
    initial_page();

    int opt, selected = 0;
    char *option[] = {"Continue game", "New game", "Log in", "Sign up", "Scoreboard", "Setting", "Quit game"};
    do {
        opt = do_menu_stuff(sizeof(option) / sizeof(option[0]), option, &selected);
        if (!strcmp("Continue game", option[opt])) {
            message_box("comming soon!");
        }
        if (!strcmp("New game", option[opt])) {
            message_box("comming soon!!");
        }
        if (!strcmp("Log in", option[opt])) {
            message_box("comming soon!!!");
        }
        if (!strcmp("Sign up", option[opt])) {
            message_box("comming soon!!!!");
        }
        if (!strcmp("Scoreboard", option[opt])) {
            message_box("comming soon!!!!!");
        }
        if (!strcmp("Setting", option[opt])) {
            message_box("comming soon!!!!!!");
        }
        if (!strcmp("Quit game", option[opt])) {
            message_box("Byeeeeeeee:_(");
        }
    } while (strcmp(option[opt], "Quit game"));


    // getch();
    endwin();
    return 0;
}
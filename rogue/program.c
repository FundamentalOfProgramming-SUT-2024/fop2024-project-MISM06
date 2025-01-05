#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#include "menu.h"

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
    user *player;
    char *user_name_current = NULL;
    do {
        opt = do_menu_stuff(sizeof(option) / sizeof(option[0]), option, &selected, user_name_current);
        if (!strcmp("Continue game", option[opt])) {
            message_box("comming soon!");
        }
        if (!strcmp("New game", option[opt])) {
            message_box("comming soon!!");
        }
        if (!strcmp("Log in", option[opt])) {
            // message_box("comming soon!!!");
            player = log_in_user();
            if (player != NULL) {
                message_box("Log in was successfull.");
                if (user_name_current == NULL) user_name_current = (char *)malloc(30 * sizeof(char));
                strcpy(user_name_current, player->username);
            }
        }
        if (!strcmp("Sign up", option[opt])) {
            user *new_user = register_user();
            if (new_user != NULL) message_box("Registered successfully, now log in to your account:)");
        }
        if (!strcmp("Scoreboard", option[opt])) {
            message_box("comming soon!!!!!");
        }
        if (!strcmp("Setting", option[opt])) {
            message_box("comming soon!!!!!!");
        }
        if (!strcmp("Quit game", option[opt])) {
            message_box("Have a good life champ :_)");
        }
    } while (strcmp(option[opt], "Quit game"));


    // getch();
    endwin();
    return 0;
}
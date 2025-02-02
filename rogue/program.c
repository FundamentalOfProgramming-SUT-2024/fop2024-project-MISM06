#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#include "menu.h"
#include "map.h"
#include "movement.h"

int main() {
    setlocale(LC_ALL, "");

    initscr();
    curs_set(0);
    noecho();
    start_color();
    use_default_colors();
    set_colors();
    init_elmnts();
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
            if (user_name_current == NULL) {
                message_box("Log in first!");
                continue;
            }
            message_box("NO game to continue!");
        }
        if (!strcmp("New game", option[opt])) {
            if (user_name_current == NULL) {
                message_box("Log in first!");
                continue;
            }
            start_a_new_game(player);
            
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
            if (user_name_current == NULL) {
                message_box("Log in first!");
                continue;
            }
            message_box("comming soon!");
        }
        if (!strcmp("Setting", option[opt])) {
            if (user_name_current == NULL) {
                message_box("Log in first!");
                continue;
            }
            open_setting(player);
        }
        if (!strcmp("Quit game", option[opt])) {
            message_box("Have a good life champ :_)");
        }
    } while (strcmp(option[opt], "Quit game"));


    // getch();
    endwin();
    return 0;
}
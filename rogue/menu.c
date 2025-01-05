#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#include "menu.h"

WINDOW* remnant = NULL;

void set_colors () {
    init_pair(RED_ON_BLACK, COLOR_RED, BLACK);
    init_pair(BLUE_ON_BLACK, COLOR_BLUE, BLACK);
    init_pair(CYAN_ON_BLACK, COLOR_CYAN, BLACK);
    init_color(PURE_YELLOW, 1000, 1000, 0);
    init_pair(YELLOW_ON_BLACK, PURE_YELLOW, BLACK);
    init_pair(GREEN_ON_BLACK, COLOR_GREEN, BLACK);
    init_color(GRAY, 500, 500, 500);
    init_color(WHITE, 1000, 1000, 1000);
    init_color(BLACK, 0, 0, 0);
    init_pair(WHITE_ON_GRAY, WHITE, GRAY);
    init_pair(WHITE_ON_BLACK, WHITE, BLACK);
    init_pair(GARY_ON_BLACK, GRAY, BLACK);

}

void str_set(char **dest, char *src, int l) {
    if (src == NULL) {
        *dest = NULL;
        return;
    }
    if (l == -1) l = strlen(src);
    if (l == 0) l = 100;
    *dest = (char *)malloc(l * sizeof(char));
    strcpy(*dest, src);
}
char* catstr(char *str1, char *str2) {
    int l1 = 0, l2 = 0;
    if (str1 != NULL) l1 = strlen(str1);
    if (str2 != NULL) l2 = strlen(str2);
    int l = l1 + l2;
    char *res = (char *)malloc(l * sizeof(char));
    if (str1 == NULL && str2 == NULL) return NULL;
    if (str1 == NULL) strcpy(res, str2);
    else if (str2 == NULL) strcpy(res, str1);
    else {
        strcpy(res, str1);
        strcat(res, str2);
    } 
    return res;
}
user* raw_user() {
    user *new_user = (user *)malloc(sizeof(user));
    str_set(&new_user->username, "raw_user", 0);
    str_set(&new_user->last_save_of_game, catstr(new_user->username, "save.json"), 0);
    str_set(&new_user->music, default_music, 0);
    str_set(&new_user->password, "", 0);
    new_user->difficulty = dif_normal;
    new_user->game_ended = 0;
    new_user->game_started = 0;
    new_user->hero_color = default_color;
    new_user->is_guest = 0;
    new_user->max_gold = 0;
    new_user->total_gold = 0;
    return new_user;
}

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
WINDOW* make_center_window() {
    int starty = (LINES - center_line) / 2;
    int startx = (COLS - center_col) / 2;
    WINDOW *win = newwin(center_line, center_col, starty, startx);
    keypad(win, true);
    wattron(win, A_DIM);
    box(win, 0, 0);
    wattroff(win, A_DIM);
    return win;
}
void message_box (char *msg) { //you can use it to interact with player
    if (remnant != NULL) {
        wclear(remnant);
        wrefresh(remnant);
        delwin(remnant);
        remnant = NULL;
    }
    WINDOW *win = newwin(3, strlen(msg) + 2, 0, 0);
    curs_set(0);
    noecho();
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
void message_box_no_end (char *msg) { //you can use it to interact with player
    if (remnant != NULL) {
        wclear(remnant);
        wrefresh(remnant);
        delwin(remnant);
    }
    WINDOW *win = newwin(3, strlen(msg) + 2, 0, 0);
    curs_set(0);
    noecho();
    keypad(win, true);
    wattron(win, A_BOLD | A_REVERSE | COLOR_PAIR(13));
    box(win, ' ', ' ');
    mvwaddstr(win, 1, 1, msg);
    wrefresh(win);
    remnant = win;
}

int do_menu_stuff (int num_op, char **option, int *slcted, char *username) {
    int selected = *slcted;
    WINDOW *menu = make_center_window();
    int ch;
    do {
        wattron(menu, COLOR_PAIR(13));
        for (int i = 0; i < num_op; i++) {
            if (i == selected) wattron(menu, A_REVERSE);
            mvwaddstr(menu, i + 1, 1, option[i]);
            if (i == selected) wattroff(menu, A_REVERSE);
        }
        wattroff(menu, COLOR_PAIR(13));
        wattron(menu, COLOR_PAIR(11));
        if (username != NULL) mvwprintw(menu, center_line - 2, 1, "Username : %s", username);
        else mvwprintw(menu, center_line - 2, 1, "No account has loged in!");
        wattroff(menu, COLOR_PAIR(11));
        wattron(menu, COLOR_PAIR(13));
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
user* register_user() {
    WINDOW *win = make_center_window();
    wattron(win, COLOR_PAIR(13));
    FILE *data = fopen("users_info.json", "r");
    if (!data) {
        data = fopen("users_info.json", "w");
        fclose(data);
        data = fopen("users_info.json", "r");
    }
    char buffer[10000];
    fread(buffer, 1, sizeof(buffer), data);
    fclose(data);
    struct json_object* users = json_tokener_parse(buffer);
    if (!users) users = json_object_new_object();
    char *msg[] = {"Email : ", "Username : ", "Password : ", "Confirm password : "};
    char *ans[10];
    int num_msg = sizeof(msg) / sizeof(msg[0]);
    for (int i = 0; i < num_msg; i++) {
        ans[i] = (char *)malloc(30 * sizeof(char));
        mvwaddstr(win, i + 1, 1, msg[i]);
    }
    for (int i = 0; i < num_msg; i++) {
        int valid = 0;
        while (!valid) {
            for (int j = 0; j < 30; j++) mvwaddch(win,i + 1, 1 + strlen(msg[i]) + j, ' ');
            wmove(win, i + 1, 1 + strlen(msg[i]));
            echo();
            curs_set(1);
            wrefresh(win);
            wgetstr(win, ans[i]);
            valid = 1;
            if (strcspn(ans[i], " \t") != strlen(ans[i])) {
                valid = 0;
                message_box("Your input shouldn't have space or tab keys, try again!");
                continue;
            }
            if (!strcmp(msg[i], "Username : ")) {
                struct json_object *garbage;
                if (json_object_object_get_ex(users, ans[i], &garbage)) {
                    message_box("Username already exist, enter new name!");
                    valid = 0;
                    continue;
                }
            }
            if (!strcmp(msg[i], "Email : ")) {
                int atsign = strcspn(ans[i], "@");
                int dot = strcspn(ans[i] + atsign, ".");
                dot += atsign;
                if (atsign == strlen(ans[i]) || dot == strlen(ans[i])) valid = 0;
                else {
                    int at2 = strcspn(ans[i] + (atsign + 1), "@");
                    int d2 = strcspn(ans[i] + (dot + 1), ".");
                    if ((dot + 1 + d2) < strlen(ans[i]) || (atsign + 1 + at2) < strlen(ans[i])) valid = 0;
                }
                if (valid == 0) {
                    message_box("Invalid Email, try again!");
                    continue;
                }
            }
            if (!strcmp(msg[i], "Password : ")) {
                if (strlen(ans[i]) < 7) {
                    valid = 0;
                    message_box("Password should be longer!");
                    continue;
                }
                int num_check = 0, cap_check = 0, small_check = 0;
                for (int j = 0; j < strlen(ans[i]); j++) {
                    if (ans[i][j] >= '0' && ans[i][j] <= '9') ++num_check;
                    if (ans[i][j] >= 'a' && ans[i][j] <= 'z') ++small_check;
                    if (ans[i][j] >= 'A' && ans[i][j] <= 'Z') ++cap_check;
                }
                if (!(num_check && cap_check && small_check)) {
                    valid = 0;
                    message_box("Password should have at least one digit, capital and small English letter!");
                    continue;
                }
            }
            if (!strcmp(msg[i], "Confirm password : ")) {
                if (strcmp(ans[i], ans[i - 1])) {
                    valid = 0;
                    message_box("Passwords didn't match!");
                    continue;
                }
            }
        }
    }
    user* new_user = (user *)malloc(sizeof(user));
    new_user->is_guest = 0;
    new_user->username =(char *)malloc(30 * sizeof(char));
    new_user->password = (char *)malloc(30 * sizeof(char));
    new_user->last_save_of_game = (char *)malloc(50 * sizeof(char));
    new_user->music = (char *)malloc(30 * sizeof(char));
    new_user->hero_color = default_color;
    new_user->total_gold = 0;
    new_user->max_gold = 0;
    new_user->game_started = 0;
    new_user->game_ended = 0;
    new_user->difficulty = dif_normal;
    strcpy(new_user->username, ans[1]);
    strcpy(new_user->password, ans[2]);
    char temp[30];
    strcpy(temp, new_user->username);
    strcat(temp, "save.json");
    strcpy(new_user->last_save_of_game, temp);
    strcpy(new_user->music, default_music);
    struct json_object *new_object = json_object_new_object();
    json_object_object_add(new_object, "password", json_object_new_string(new_user->password));
    json_object_object_add(new_object, "total_gold", json_object_new_int(new_user->total_gold));
    json_object_object_add(new_object, "max_gold", json_object_new_int(new_user->max_gold));
    json_object_object_add(new_object, "game_started", json_object_new_int(new_user->game_started));
    json_object_object_add(new_object, "game_ended", json_object_new_int(new_user->game_ended));
    json_object_object_add(new_object, "is_guest", json_object_new_int(new_user->is_guest));
    json_object_object_add(new_object, "difficulty", json_object_new_int(new_user->difficulty));
    json_object_object_add(new_object, "last_save_of_game", json_object_new_string(new_user->last_save_of_game));
    json_object_object_add(new_object, "music", json_object_new_string(new_user->music));
    json_object_object_add(new_object, "hero_color", json_object_new_int(new_user->hero_color));
    json_object_object_add(users, new_user->username, new_object);

    data = fopen("users_info.json", "w");
    if (!data) {
        message_box("OPS, something happed, please close the game and try again!");
        return NULL;
    }
    fprintf(data, "%s\n", json_object_to_json_string(users));
    fclose(data);
    noecho();
    curs_set(0);
    wrefresh(win);
    wclear(win);
    wrefresh(win);
    delwin(win);
    for (int i = 0; i < num_msg; i++) free(ans[i]);
    json_object_put(users);
    return new_user;
}
user* log_in_user() {
    WINDOW *win = make_center_window();
    wattron(win, COLOR_PAIR(13));
    mvwaddstr(win, 1, 1, "For log in as 'guest' press 'F1'");
    wrefresh(win);
    int chh = wgetch(win);
    if (chh == KEY_F(1)) {
        user* new_user = (user *)malloc(sizeof(user));
        new_user->username =(char *)malloc(30 * sizeof(char));
        new_user->password = (char *)malloc(30 * sizeof(char));
        // new_user->last_save_of_game = (char *)malloc(30 * sizeof(char));
        new_user->music = (char *)malloc(30 * sizeof(char));
        new_user->hero_color = default_color;
        new_user->total_gold = 0;
        new_user->max_gold = 0;
        new_user->game_started = 0;
        new_user->game_ended = 0;
        new_user->is_guest = 1;
        new_user->difficulty = dif_normal;
        strcpy(new_user->username, "GUEST");
        strcpy(new_user->password, "");
        // strcpy(new_user->last_save_of_game, strcat(new_user->username, "save.json"));
        strcpy(new_user->music, default_music);
        wclear(win);
        wrefresh(win);
        delwin(win);
        // message_box("You've loged in as GUEST");
        return new_user;
    }
    FILE *data = fopen("users_info.json", "r");
    if (!data) {
        message_box("There is no registered username!");
        delwin(win);
        return NULL;
    }
    char buffer[1000];
    if (!data) {
        message_box("goh");
    }
    fread(buffer, 1, sizeof(buffer), data);
    fclose(data);
    struct json_object* users = json_tokener_parse(buffer);
    if (!users) users = json_object_new_object();
    char *msg[] = {"Username : ", "Password : "};
    char *ans[10];
    int num_msg = sizeof(msg) / sizeof(msg[0]);
    for (int i = 0; i < num_msg; i++) {
        ans[i] = (char *)malloc(30 * sizeof(char));
        mvwaddstr(win, i + 1, 1, msg[i]);
    }
    struct json_object *user_object = json_object_new_object();
    for (int i = 0; i < num_msg; i++) {
        int valid = 0;
        while (!valid) {
            for (int j = 0; j < 30; j++) mvwaddch(win,i + 1, 1 + strlen(msg[i]) + j, ' ');
            wmove(win, i + 1, 1 + strlen(msg[i]));
            echo();
            curs_set(1);
            wrefresh(win);
            wgetstr(win, ans[i]);
            valid = 1;
            if (!strcmp(msg[i], "Username : ")) {
                struct json_object *garb;
                if (!json_object_object_get_ex(users, ans[i], &garb)) {
                    message_box("Username don't exist, register first!");
                    wclear(win);
                    wrefresh(win);
                    delwin(win);
                    for (int i = 0; i < num_msg; i++) free(ans[i]);
                    json_object_put(users);
                    return NULL;
                } else {
                    user_object = json_object_object_get(users, ans[i]);
                }
            }
            if (!strcmp(msg[i], "Password : ")) {
                if (strcmp(ans[i], json_object_get_string(json_object_object_get(user_object, "password")))) {
                    valid = 0;
                    message_box("Incorrect password");
                }
            }
        }
    }
    user* new_user = (user *)malloc(sizeof(user));
    new_user->username =(char *)malloc(30 * sizeof(char));
    new_user->password = (char *)malloc(30 * sizeof(char));
    new_user->last_save_of_game = (char *)malloc(30 * sizeof(char));
    new_user->music = (char *)malloc(30 * sizeof(char));
    new_user->hero_color = json_object_get_int(json_object_object_get(user_object, "hero_color"));
    new_user->total_gold = json_object_get_int(json_object_object_get(user_object, "total_gold"));
    new_user->max_gold = json_object_get_int(json_object_object_get(user_object, "max_gold"));
    new_user->game_started = json_object_get_int(json_object_object_get(user_object, "game_started"));
    new_user->game_ended = json_object_get_int(json_object_object_get(user_object, "game_ended"));
    new_user->difficulty = json_object_get_int(json_object_object_get(user_object, "difficulty"));
    new_user->is_guest = 0;
    strcpy(new_user->username, ans[0]);
    strcpy(new_user->password, ans[1]);
    strcpy(new_user->last_save_of_game, json_object_get_string(json_object_object_get(user_object, "last_save_of_game")));
    strcpy(new_user->music, json_object_get_string(json_object_object_get(user_object, "music")));
    
    noecho();
    curs_set(0);
    wrefresh(win);
    wclear(win);
    wrefresh(win);
    delwin(win);
    for (int i = 0; i < num_msg; i++) free(ans[i]);
    json_object_put(users);
    return new_user;
}
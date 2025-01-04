#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<json-c/json.h>

#define center_line 20
#define center_col 60
#define dif_easy 0
#define dif_normal 1
#define dif_hard 2
#define default_color 0
#define default_music ""

typedef struct user_type {
    char *username;
    char *password;
    int total_gold;
    int max_gold;
    int game_started;
    int game_ended;
}user;

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
int do_menu_stuff (int num_op, char **option, int *slcted) {
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
    // keypad(win, false);
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
    new_user->username =(char *)malloc(30 * sizeof(char));
    new_user->password = (char *)malloc(30 * sizeof(char));
    new_user->total_gold = 0;
    new_user->max_gold = 0;
    new_user->game_started = 0;
    new_user->game_ended = 0;
    strcpy(new_user->username, ans[1]);
    strcpy(new_user->password, ans[2]);
    
    struct json_object *new_object = json_object_new_object();
    json_object_object_add(new_object, "password", json_object_new_string(new_user->password));
    json_object_object_add(new_object, "total_gold", json_object_new_int(new_user->total_gold));
    json_object_object_add(new_object, "max_gold", json_object_new_int(new_user->max_gold));
    json_object_object_add(new_object, "game_started", json_object_new_int(new_user->game_started));
    json_object_object_add(new_object, "game_ended", json_object_new_int(new_user->game_ended));
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
    return new_user;
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
    user *player;
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
            message_box("Byeeeeeeee:_(");
        }
    } while (strcmp(option[opt], "Quit game"));


    // getch();
    endwin();
    return 0;
}
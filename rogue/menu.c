#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>

#include "menu.h"

WINDOW* remnant = NULL;

void set_colors () {
    init_color(PURE_YELLOW, 1000, 1000, 0);
    init_color(GRAY, 500, 500, 700);
    init_color(WHITE, 1000, 1000, 1000);
    init_color(BLACK, 0, 0, 0);
    init_color(RED, 700, 0, 100);
    init_color(PURPLE_DARK, 500, 0, 500);
    init_color(DARK_BLUE, 0, 0, 1000); 

    init_pair(BLUE_D_ON_BLACK, DARK_BLUE, BLACK);
    init_pair(PURPLE_D_ON_BLACK, PURPLE_DARK, BLACK);
    init_pair(RED_ON_BLACK, RED, BLACK);
    init_pair(BLUE_ON_BLACK, COLOR_BLUE, BLACK);
    init_pair(CYAN_ON_BLACK, COLOR_CYAN, BLACK);
    init_pair(YELLOW_ON_BLACK, PURE_YELLOW, BLACK);
    init_pair(GREEN_ON_BLACK, COLOR_GREEN, BLACK);
    init_pair(WHITE_ON_GRAY, WHITE, GRAY);
    init_pair(WHITE_ON_BLACK, WHITE, BLACK);
    init_pair(GRAY_ON_BLACK, GRAY, BLACK);
    init_pair(RED_ON_GRAY, RED, GRAY);
    init_pair(GREEN_ON_GRAY, COLOR_GREEN, GRAY);
    init_pair(YELLOW_ON_GRAY, PURE_YELLOW, GRAY);    

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
char* catnum(char *str, int x) {
    int l = 0;
    char *s = (char *)malloc(2 * sizeof(char));
    s[1] = '\0';
    s[0] = x + '0';
    if (str == NULL) return s;
    ++l;
    char *res = (char *)malloc(l * sizeof(char));
    strcpy(res, str);
    strcat(res, s);
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
    new_user->is_music_on = 1;
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
    wattron(win, A_DIM | COLOR_PAIR(BLUE_ON_BLACK));
    box(win, 0, 0);
    wattroff(win, A_DIM | COLOR_PAIR(BLUE_ON_BLACK));
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

int open_choosing(user *player, char **options, int n) {
    WINDOW *win = make_center_window();
    int selected = 0;
    int num_op = n;
    int ch;
    wattron(win, COLOR_PAIR(GRAY_ON_BLACK) | A_BOLD);
    mvwprintw(win, 0, getmaxx(win) - 10 , "Setting");
    wattroff(win, COLOR_PAIR(GRAY_ON_BLACK) | A_BOLD);
    do {
        wattron(win, COLOR_PAIR(13));
        for (int i = 0; i < num_op; i++) {
            if (i == selected) wattron(win, A_REVERSE);
            mvwprintw(win, i + 1, 1, "%s", options[i]);
            if (i == selected) wattroff(win, A_REVERSE);
        }
        wrefresh(win);
        ch = wgetch(win);
        switch (ch) {
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
            case KEY_F(1): selected = -1; break;
            default :
                message_box("Use up and down keys!");
        }
    } while (ch != 10 && selected >= 0);
    wclear(win);
    wrefresh(win);
    delwin(win);
    return selected;
}

void open_setting (user *player) {
    char *options[] = {"Change Difficulty", "Change color of hero", "Change music", "Disable music", "Enable music"};
    int slct;
    do {
        slct = open_choosing(player, options, 5);
        switch(slct) {
            case 0:
                char *opt2[] = {"EASY", "NORMAL", "HARD"};
                int t = open_choosing(player, opt2, 3);
                switch(t) {
                    case 0:
                        player->difficulty = dif_easy;
                        break;
                    case 1:
                        player->difficulty = dif_normal;
                        break;
                    case 2:
                        player->difficulty = dif_hard;
                        break;
                    default : break;
                }
                break;
            case 1:
                char *opt3[] = {"WHITE (default)", "RED", "YELLOW", "BLUE"};
                int t2 = open_choosing(player, opt3, 4);
                switch(t2) {
                    case 0:
                        player->hero_color = 18;
                        break;
                    case 1:
                        player->hero_color = 11;
                        break;
                    case 2:
                        player->hero_color = 14;
                        break;
                    case 3:
                        player->hero_color = 12;
                    default : break;
                }
                break;
            case 2:
                message_box("coming soon!");
                break;
            case 3:
                player->is_music_on = 0;
                break;
            case 4:
                player->is_music_on = 1;
                break;
            default : break;
        }
    } while (slct >= 0);
}

void scoreboard_show (user *player) {
    FILE *data = fopen("users_info.json", "r");
    if (!data) {
        message_box("There is no registered username!");
        return;
    }
    char buffer[100000];
    size_t fsz = fread(buffer, 1, sizeof(buffer), data);
    fclose(data);
    buffer[fsz] = '\0';
    struct json_object* users = json_tokener_parse(buffer);
    if (!users) {
        message_box("No valid user!");
        return;
    }
    int n = 0;
    char username[10000][100];
    int total_gold[10000];
    int max_gold[10000];
    int game_ended[10000];
    int game_started[10000];
    int rank[10000];
    
    json_object_object_foreach(users, name, prof) {
        username[n][0] = '\0';
        strcpy(username[n], name);
        total_gold[n] = json_object_get_int(json_object_object_get(prof, "total_gold"));
        max_gold[n] = json_object_get_int(json_object_object_get(prof, "max_gold"));
        game_ended[n] = json_object_get_int(json_object_object_get(prof, "game_ended"));
        game_started[n] = total_gold[n] = json_object_get_int(json_object_object_get(prof, "total_gold"));
        rank[n] = n;
        ++n;
    }
    json_object_put(users);

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (max_gold[rank[i]] < max_gold[rank[j]]) {
                int temp = rank[i];
                rank[i] = rank[j];
                rank[j] = temp;
            } else if (max_gold[rank[i]] == max_gold[rank[j]] && total_gold[rank[i]] < total_gold[rank[j]]) {
                int temp = rank[i];
                rank[i] = rank[j];
                rank[j] = temp;
            }
        }
    }
    int view = 20;
    int page = 0;
    int page_num = (n + (view - 1)) / view;
    
    int lny = view + 6;
    int lnx = 61;
    int sty = (LINES - lny) / 2;
    int stx = (COLS - lnx) / 2;
    WINDOW *win = newwin(lny, lnx, sty, stx);
    //rank 1 to 4
    //username 6 to 30
    //point 32 to 36
    //sum pt 38 to 43
    //exp 45 to 48
    //tried 50 52
    // nothing 54 to 59
    
    int ch;
    do {
        wclear(win);
        wattron(win, COLOR_PAIR(RED_ON_BLACK));
        box(win, 0, 0);
        wattron(win, A_BOLD);
        mvwprintw(win, 0, lnx - 13, "Scoreboard");
        wattroff(win, A_BOLD);
        mvwprintw(win, lny - 2, 1, "Page %d / %d", page + 1, page_num);
        for (int i = 1; i < lnx - 1; i++) {
            mvwaddch(win, 2, i, ACS_HLINE);
        }
        mvwprintw(win, 1, 1, "Rank");
        // mvwaddch(win, 1, 5, ACS_VLINE);
        mvwprintw(win, 1, 6, "        Username");
        // mvwaddch(win, 1, 31, ACS_VLINE);
        mvwprintw(win, 1, 32, "Point");
        // mvwaddch(win, 1, 37, ACS_VLINE);
        mvwprintw(win, 1, 38, "Sum pt");
        // mvwaddch(win, 1, 44, ACS_VLINE);
        mvwprintw(win, 1, 45, "Exp");
        // mvwaddch(win, 1, 49, ACS_VLINE);
        mvwprintw(win, 1, 50, "Try");
        // mvwaddch(win, 1, 53, ACS_VLINE);


        wattroff(win, COLOR_PAIR(RED_ON_BLACK));

        int rf = view * (page + 1);
        if (n < rf) rf = n;
        int kf = view * page;
        for (int i = view * page; i < rf; i++) {
            int id = rank[i];
            char nick[10]; nick[0] = '\0';
            chtype attr = A_ITALIC;
            if (!strcmp(username[id], player->username)) {
                attr |= A_REVERSE;
            }
            if (i == 0) {
                attr |= COLOR_PAIR(YELLOW_ON_BLACK);
                
            } else if (i == 1) {
                attr |= COLOR_PAIR(BLUE_ON_BLACK);
            } else if (i == 2) {
                attr |= COLOR_PAIR(GREEN_ON_BLACK);
            } else {
                attr |= COLOR_PAIR(GRAY_ON_BLACK);
            }
            wattron(win, attr);
            for (int j = 1; j < lnx - 1; j++) mvwprintw(win, i + 3, j, " ");
            mvwprintw(win, i + 3 - kf, 1, "%d", i);
            // mvwprintw(win, i + 3, 5, "|");
            mvwprintw(win, i + 3 - kf, 6, "%s", username[id]);
            // mvwprintw(win, i + 3, 31, "|");
            mvwprintw(win, i + 3 - kf, 32, "%d", max_gold[id]);
            // mvwprintw(win, i + 3, 37, "|");
            mvwprintw(win, i + 3 - kf, 38, "%d", total_gold[id]);
            // mvwprintw(win, i + 3, 44, "|");
            mvwprintw(win, i + 3 - kf, 45, "%d", game_ended[id]);
            // mvwprintw(win, i + 3, 49, "|");
            mvwprintw(win, i + 3 - kf, 50, "%d", game_started[id]);
            // mvwprintw(win, i + 3, 53, "|");
            if (i == 0) mvwprintw(win, i + 3 - kf, 54, "LEGENG");
            if (i == 1) mvwprintw(win, i + 3 - kf, 54, "MASTER");
            if (i == 2) mvwprintw(win, i + 3 - kf, 54, "EXPERT");
            wattroff(win, attr);
        }
        wrefresh(win);
        ch = getch();
        switch(ch) {
            case KEY_RIGHT :
                ++page;
                page %= page_num;
                break;
            case KEY_LEFT :
                page += page_num;
                --page;
                page %= page_num;
                break;
            case KEY_F(1) :
                break;
            default :
                message_box("F1 for back to menu, arrow key for changing page.");
                break;
        } 

    }while (ch != KEY_F(1));
    wclear(win);
    wrefresh(win);
    delwin(win);

}

void open_profile (user *player) {
    WINDOW *win = make_center_window();
    wattron(win, A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(win, 0, getmaxx(win) - 10, "Profile");
    wattroff(win, A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));

    wattron(win, COLOR_PAIR(WHITE_ON_BLACK));
    wmove(win, 1, 1);
    wprintw(win, "Username: ");
    wattroff(win, COLOR_PAIR(WHITE_ON_BLACK));
    wattron(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);
    wprintw(win, "%s", player->username);
    wattroff(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);

    wattron(win, COLOR_PAIR(WHITE_ON_BLACK));
    wmove(win, 2, 1);
    wprintw(win, "Point: ");
    wattroff(win, COLOR_PAIR(WHITE_ON_BLACK));
    wattron(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);
    wprintw(win, "%d", player->max_gold);
    wattroff(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);

    wattron(win, COLOR_PAIR(WHITE_ON_BLACK));
    wmove(win, 3, 1);
    wprintw(win, "Sum pt: ");
    wattroff(win, COLOR_PAIR(WHITE_ON_BLACK));
    wattron(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);
    wprintw(win, "%d", player->total_gold);
    wattroff(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);

    wattron(win, COLOR_PAIR(WHITE_ON_BLACK));
    wmove(win, 4, 1);
    wprintw(win, "Experience: ");
    wattroff(win, COLOR_PAIR(WHITE_ON_BLACK));
    wattron(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);
    wprintw(win, "%d", player->game_ended);
    wattroff(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);

    wattron(win, COLOR_PAIR(WHITE_ON_BLACK));
    wmove(win, 5, 1);
    wprintw(win, "Tries: ");
    wattroff(win, COLOR_PAIR(WHITE_ON_BLACK));
    wattron(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);
    wprintw(win, "%d", player->game_started);
    wattroff(win, COLOR_PAIR(RED_ON_BLACK) | A_ITALIC);

    wrefresh(win);
    int ch;
    do {
        ch = getch();
    } while (ch != KEY_F(1));

    wclear(win);
    wrefresh(win);
    delwin(win);

}

int do_menu_stuff (int num_op, char **option, int *slcted, char *username) {
    int selected = *slcted;
    WINDOW *menu = make_center_window();
    wattron(menu, COLOR_PAIR(GRAY_ON_BLACK) | A_BOLD);
    mvwprintw(menu, 0, getmaxx(menu) - 8, "Menu");
    wattroff(menu, COLOR_PAIR(GRAY_ON_BLACK) | A_BOLD);
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
    wrefresh(menu);
    delwin(menu);
    *slcted = selected;
    return selected;
}
void save_player_infos (user *player) {
    if (player->is_guest) return;
    FILE *data = fopen("users_info.json", "r");
    if (!data) {
        data = fopen("users_info.json", "w");
        fclose(data);
        data = fopen("users_info.json", "r");
    }
    char buffer[100000];
    size_t fsz = fread(buffer, 1, sizeof(buffer), data);
    fclose(data);
    buffer[fsz] = '\0';
    struct json_object* users = json_tokener_parse(buffer);
    if (!users) users = json_object_new_object();
    else json_object_object_del(users, player->username);

    struct json_object *new_object = json_object_new_object();
    json_object_object_add(new_object, "password", json_object_new_string(player->password));
    json_object_object_add(new_object, "total_gold", json_object_new_int(player->total_gold));
    json_object_object_add(new_object, "max_gold", json_object_new_int(player->max_gold));
    json_object_object_add(new_object, "game_started", json_object_new_int(player->game_started));
    json_object_object_add(new_object, "game_ended", json_object_new_int(player->game_ended));
    json_object_object_add(new_object, "is_guest", json_object_new_int(player->is_guest));
    json_object_object_add(new_object, "difficulty", json_object_new_int(player->difficulty));
    json_object_object_add(new_object, "last_save_of_game", json_object_new_string(player->last_save_of_game));
    json_object_object_add(new_object, "music", json_object_new_string(player->music));
    json_object_object_add(new_object, "hero_color", json_object_new_int(player->hero_color));
    json_object_object_add(new_object, "is_music_on", json_object_new_int(player->is_music_on));
    json_object_object_add(users, player->username, new_object);

    data = fopen("users_info.json", "w");
    if (!data) {
        message_box("OPS, something happed, please close the game and try again!");
    } else {
        fprintf(data, "%s\n", json_object_to_json_string(users));
        fclose(data);
    }
    json_object_put(users);

}
user* register_user() {
    WINDOW *win = make_center_window();
    wattron(win, COLOR_PAIR(GRAY_ON_BLACK) | A_BOLD);
    mvwprintw(win, 0, getmaxx(win) - 11, "Register");
    wattroff(win, COLOR_PAIR(GRAY_ON_BLACK) | A_BOLD);
    wattron(win, COLOR_PAIR(13));
    FILE *data = fopen("users_info.json", "r");
    if (!data) {
        data = fopen("users_info.json", "w");
        fclose(data);
        data = fopen("users_info.json", "r");
    }
    char buffer[100000];
    size_t fsz = fread(buffer, 1, sizeof(buffer), data);
    fclose(data);
    buffer[fsz] = '\0';
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
    new_user->hero_color = COLOR_PAIR(YELLOW_ON_BLACK);
    new_user->total_gold = 0;
    new_user->max_gold = 0;
    new_user->game_started = 0;
    new_user->game_ended = 0;
    new_user->difficulty = dif_normal;
    new_user->is_music_on = 1;
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
    json_object_object_add(new_object, "is_music_on", json_object_new_int(new_user->is_music_on));
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
    wattron(win, COLOR_PAIR(GRAY_ON_BLACK) | A_BOLD);
    mvwprintw(win, 0, getmaxx(win) - 9, "Log in");
    wattroff(win, COLOR_PAIR(GRAY_ON_BLACK) | A_BOLD);
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
    char buffer[100000];
    if (!data) {
        message_box("goh");
    }
    size_t fsz = fread(buffer, 1, sizeof(buffer), data);
    fclose(data);
    buffer[fsz] = '\0';
    // mvprintw(0, 0, "%lu", fsz);
    getch();
    struct json_object* users = json_tokener_parse(buffer);
    if (!users) {
        users = json_object_new_object();
        message_box("nothing");
    }
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
                // message_box(ans[i]);
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
    new_user->is_music_on = json_object_get_int(json_object_object_get(user_object, "is_music_on"));
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
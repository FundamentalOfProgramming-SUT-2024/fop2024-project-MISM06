#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>

#ifndef menu_h_defined
#define menu_h_defined

#define center_line 20
#define center_col 60
#define dif_easy 0
#define dif_normal 1
#define dif_hard 2
#define default_color 0
#define default_music ""

typedef struct user_type {
    int is_guest;
    char *username;
    char *password;
    int total_gold;
    int max_gold;
    int game_started;
    int game_ended;
    char *last_save_of_game;
    int difficulty;
    char *music;
    int hero_color;
} user;

void initial_page();
WINDOW* make_center_window();
void message_box (char *msg);
int do_menu_stuff (int num_op, char **option, int *slcted, char *username);
user* register_user();
user* log_in_user();

#endif
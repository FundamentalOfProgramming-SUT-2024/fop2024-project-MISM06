#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#ifndef menu_h_defined
#define menu_h_defined

#define center_line 20
#define center_col 60
#define dif_easy 0
#define dif_normal 1
#define dif_hard 2
#define default_color 18
#define default_music ""
#define RED_ON_BLACK 11
#define BLUE_ON_BLACK 12
#define CYAN_ON_BLACK 13
#define YELLOW_ON_BLACK 14
#define GREEN_ON_BLACK 15
#define PURE_YELLOW 21
#define WHITE_ON_GRAY 16
#define GRAY_ON_BLACK 17
#define WHITE_ON_BLACK 18
#define GRAY 22
#define WHITE 23
#define BLACK 24
#define RED_ON_GRAY 19
#define GREEN_ON_GRAY 25
#define RED 26
#define YELLOW_ON_GRAY 27
#define PURPLE_DARK 28
#define PURPLE_D_ON_BLACK 29
#define DARK_BLUE 30
#define BLUE_D_ON_BLACK 31

extern WINDOW* remnant;
extern Mix_Music *music;

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
    int is_music_on;
} user;

void set_colors ();
int get_rand2(int l, int r);
void play_music (char *name);
void str_set(char **dest, char *src, int l);
char* catstr(char *str1, char *str2);
char* catnum(char *str, int x);
user* raw_user();

void initial_page();
WINDOW* make_center_window();
void message_box (char *msg);
void message_box_no_end (char *msg);
int open_choosing(user *player, char **options, int n);
void open_setting (user *player);
void scoreboard_show (user *player);
void open_profile (user *player);
int do_menu_stuff (int num_op, char **option, int *slcted, char *username);
void save_player_infos (user *player);
user* register_user();
user* log_in_user();

#endif
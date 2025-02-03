#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "menu.h"

#ifndef map_h_defined
#define map_h_defined
extern int line_lvl;
extern int col_lvl;
extern int starty_lvl;
extern int startx_lvl;
extern int delta_y;
extern int delta_x;
extern chtype h_line;
extern chtype v_line;
extern chtype ul_corner;
extern chtype ur_corner;
extern chtype ll_corner;
extern chtype lr_corner;
extern chtype el_door;
extern chtype el_floor;
extern chtype el_road;
extern chtype el_window;
extern chtype el_trap;
extern chtype el_stair;
extern chtype el_stair_up;
extern chtype el_hidden_door;

extern char *gold_regular;
extern char *gold_black; //each 5 time reg;
extern chtype gold_regular_attr;
extern chtype gold_black_attr;

extern char *talisman_health;
extern char *talisman_speed;
extern char *talisman_damage;
extern chtype talisman_health_attr;
extern chtype talisman_speed_attr;
extern chtype talisman_damage_attr;

extern char *weapon_sword;
extern char *weapon_mace; //gorz
extern char *weapon_dagger; //khankar
extern char *weapon_magic_wand;
extern char *weapon_arrow;
extern chtype weapon_sword_attr;
extern chtype weapon_mace_attr;
extern chtype weapon_dagger_attr;
extern chtype weapon_magic_wand_attr;
extern chtype weapon_arrow_attr;

extern char *food_reg;
extern char *food_golden; //aala
extern char *food_magical;
extern char *food_rotten;
extern chtype food_reg_attr;
extern chtype food_golden_attr;
extern chtype food_magical_attr;
extern chtype food_rotten_attr;

extern char *hero;
extern chtype hero_attr;

void init_elmnts ();

typedef struct element_type {
    int y;
    int x;
} elmnt; 

typedef struct room_type {
    int starty;
    int startx;
    int h;
    int w;
    int door_num;
    elmnt *door;
    // elmnt wndow;
    // elmnt trap;
    // elmnt stair;
    // elmnt hidden_door;
} room;

typedef struct pickable_things_type {
    int **pickable_sit; //0 nothing; 1 pickable; 2 ilusion;
    char ***cells; // def '\0'
    chtype **attr; // def 0
    int **cnt; // def 0
} pickable_things;

typedef struct enemy_type {
    char look;
    char *name;
    chtype attr;
    int max_hp;
    int max_token;
    int damage;
    int **cnt;
    int **hp;
    int **token;
} enemy;

typedef struct lvl_type {
    elmnt up_stair;
    elmnt down_stair;
    int room_num;
    int reg_room_num;
    room** rooms;
    chtype **cell; // def 0
    chtype **hidden_cell; // def 0

    pickable_things *golds;
    pickable_things *weapons;
    pickable_things *talismans;
    pickable_things *foods;
    
    int **hidden_sit; //0 nothing; 1 hidden; 2 founded;
    int **explore_sit; //0 hasnt explored; 1 explored;
    int **room_id; //-1 not room; else for room;

    int nightmare_room_id; //purple
    int treasure_room_id; //golden
    int enchant_room_id; //blue

    int monster_num;
    enemy **monster; // 0:deamon, 1 fire breathing, 2 giant, 3 snake, 4 undead; 

} lvl;

typedef struct INVENTORY_type {

    int gold_cnt;

    int food_golden_cnt;
    int food_magical_cnt;
    int food_reg_cnt;
    int food_rotten_cnt;
    char *food_def;
    chtype food_def_attr;

    int weapon_arrow_cnt;
    int weapon_dagger_cnt;
    int weapon_mace_cnt;
    int weapon_magic_wand_cnt;
    int weapon_sword_cnt;
    char *weapon_def;
    chtype weapon_def_attr;

    int talisman_damage_cnt;
    int talisman_health_cnt;
    int talisman_speed_cnt;
    char *talisman_def;
    chtype talisman_def_attr;

} Inventory;

typedef struct map_type {
    int curr_lvl;
    elmnt hero_place;
    int lvl_num;
    lvl** lvls;
    Inventory *inv;
    int time;
    int hp, stamina, enchant_speed, enchant_damage;
} map;



int get_rand(int l, int r);

room* make_room(int h, int w, int door_num);

int is_wall(chtype ch);

int is_this_floor(chtype ch);

int is_filled(lvl *lv, int y, int x);

int is_adjance(int y1, int x1, int y2, int x2);

int dont_cover_doors(room *rm, int y, int x);

int is_floor (lvl *lv, int y, int x);

void add_cell(chtype ch, lvl *lv, int y, int x, int hidden_sit);

void set_el_in_room(chtype el_, lvl *lv, int rm_id, int cnt, int hidden_sit);

elmnt * give_set_el_in_room(lvl *lv, int rm_id);

elmnt * give_free_elmnt_in_room(lvl *lv, int rm_id);

int over_lap (int h1, int w1, int y1, int x1, int h2, int w2, int y2, int x2);

void make_border(chtype atr, chtype **cell, int sy, int sx, int h, int w);

void fill_inside(chtype ch, chtype **cell, int sy, int sx, int h, int w);

void fill_inside_int(int ch, int **arr, int sy, int sx, int h, int w);

void bfs(lvl *lv, chtype **cell, elmnt src, elmnt dest, int **mark);

void make_path (lvl *lv, chtype **cell, elmnt src, elmnt dest, int diff);

void make_roads (lvl *lv, int diff, int reg_room_num);

lvl* make_lvl(int diff, int is_first_lvl, int is_last_lvl);

void add_pickable_things (int diff, lvl *lv);

map* generate_map(user *player);

void save_map (map *mp, user *player);

map* load_map (user *player);

#endif
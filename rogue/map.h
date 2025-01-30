#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>

#include "menu.h"

#ifndef map_h_defined
#define map_h_defined
extern int line_lvl;
extern int col_lvl;
extern int starty_lvl;
extern int startx_lvl;
extern int delta;
extern chtype h_line;
extern chtype v_line;
extern chtype ul_corner;
extern chtype ur_corner;
extern chtype ll_corner;
extern chtype lr_corner;
extern chtype el_door;
extern chtype el_pillar;
extern chtype el_floor;
extern chtype el_road;
extern chtype el_window;
extern chtype el_trap;
extern chtype el_stair;
extern chtype el_hidden_door;
extern chtype el_password_door; //locked : red; unlocked : green;
extern chtype el_password_door_unlocked;
extern chtype el_password_maker;

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
    // elmnt pillar;
    // elmnt wndow;
    // elmnt trap;
    // elmnt stair;
} room;

typedef struct lvl_type {
    int room_num;
    room** rooms;
    chtype **cell;
    chtype **hidden_cell;
    int **hidden_sit; //0 nothing; 1 hidden; 2 founded;
    // int **room_id;
} lvl;

typedef struct map_type {
    int lvl_num;
    lvl** lvls;
} map;

void save_map (map *mp, user *player);

map* load_map (user *player);

int get_rand(int l, int r);

room* make_room(int h, int w, int door_num);

int is_wall(chtype ch);

int is_adjance(int y1, int x1, int y2, int x2);

int dont_cover_doors(room *rm, int y, int x);

int is_floor (lvl *lv, int y, int x);

void add_cell(chtype ch, lvl *lv, int y, int x, int hidden_sit);

void set_el_in_room(chtype el_, lvl *lv, int rm_id, int cnt, int hidden_sit);

int over_lap (int h1, int w1, int y1, int x1, int h2, int w2, int y2, int x2);

void make_border(chtype **cell, int sy, int sx, int h, int w);

void fill_inside(chtype ch, chtype **cell, int sy, int sx, int h, int w);

void bfs(lvl *lv, chtype **cell, elmnt src, elmnt dest, int **mark);

void make_path (lvl *lv, chtype **cell, elmnt src, elmnt dest, int diff);

void make_roads (lvl *lv, int diff);

lvl* make_lvl(int diff);

map* generate_map(user *player);

#endif
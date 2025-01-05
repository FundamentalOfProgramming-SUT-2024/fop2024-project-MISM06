#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>

#include "menu.h"

int line_lvl;
int col_lvl;
int starty_lvl;
int startx_lvl;
int delta;
chtype h_line;
chtype v_line;
chtype ul_corner;
chtype ur_corner;
chtype ll_corner;
chtype lr_corner;
chtype el_door;
chtype el_pillar;
chtype el_floor;
chtype el_road;

void init_elmnts () {
    srand(time(0));
    remnant = NULL;
    delta = 5;
    line_lvl = LINES - delta - delta;
    col_lvl = COLS - delta - delta;
    starty_lvl = delta;
    startx_lvl = delta;
    h_line = ACS_HLINE | A_REVERSE | COLOR_PAIR(RED_ON_BLACK);
    v_line = ACS_VLINE | A_REVERSE | COLOR_PAIR(RED_ON_BLACK);
    ul_corner = ACS_ULCORNER | A_REVERSE | COLOR_PAIR(RED_ON_BLACK);
    ur_corner = ACS_URCORNER | A_REVERSE | COLOR_PAIR(RED_ON_BLACK);
    ll_corner = ACS_LLCORNER | A_REVERSE | COLOR_PAIR(RED_ON_BLACK);
    lr_corner = ACS_LRCORNER | A_REVERSE | COLOR_PAIR(RED_ON_BLACK);
    el_door = ACS_PLUS | A_REVERSE | COLOR_PAIR(GREEN_ON_BLACK);
    el_pillar = 'O' | COLOR_PAIR(WHITE_ON_GRAY);
    el_floor = '.' | COLOR_PAIR(GARY_ON_BLACK) | A_REVERSE;
    el_road = '#' | COLOR_PAIR(GARY_ON_BLACK) | A_REVERSE;
}

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
    int pillar_num;
    elmnt *door;
    elmnt *pillar;
} room;

typedef struct lvl_type {
    int room_num;
    room** rooms;
    chtype **cell;
    int **access;
    int **room_id;
} lvl;

typedef struct map_type {
    int lvl_num;
    lvl** lvls;
} map;

int get_rand(int l, int r) {
    return l + (rand() % (r - l + 1));
}

room* make_room(int h, int w, int door_num, int pillar_num) {
    room* rm = (room *)malloc(sizeof(room));
    rm->h = h;
    rm->w = w;
    rm->door_num = door_num;
    rm->pillar_num = pillar_num;
    rm->door = (elmnt *)malloc(door_num * sizeof(elmnt));
    rm->pillar = (elmnt *)malloc(pillar_num * sizeof(elmnt));
    for (int i = 0; i < door_num; i++) {
        rm->door[i].y = -1;
        rm->door[i].x = -1;
    }
    for (int i = 0; i < pillar_num; i++) {
        rm->pillar[i].y = -1;
        rm->pillar[i].x = -1;
    }
    return rm;
}


int over_lap (int h1, int w1, int y1, int x1, int h2, int w2, int y2, int x2) {
    int ey1 = y1 + h1 - 1;
    int ex1 = x1 + w1 - 1;
    int ey2 = y2 + h2 - 1;
    int ex2 = x2 + w2 - 1;
    --x1; --y1; ++ex1; ++ey1;
    --x2; --y2; ++ex2; ++ey2;
    if (y1 > ey2) return 0;
    if (ey1 < y2) return 0;
    if (x1 > ex2) return 0;
    if (ex1 < x2) return 0;
    return 1;
}

void make_border(chtype **cell, int sy, int sx, int h, int w) {
    int ey = sy + h - 1;
    int ex = sx + w - 1;
    cell[sy][sx] = ul_corner;
    cell[sy][ex] = ur_corner;
    cell[ey][sx] = ll_corner;
    cell[ey][ex] = lr_corner;
    for (int i = sy + 1; i < ey; i++) {
        cell[i][sx] = v_line;
        cell[i][ex] = v_line;
    }
    for (int i = sx + 1; i < ex; i++) {
        cell[sy][i] = h_line;
        cell[ey][i] = h_line;
    }
}

void fill_inside(chtype ch, chtype **cell, int sy, int sx, int h, int w) {
    int ey = sy + h - 1;
    int ex = sx + w - 1;
    for (int i = sy; i <= ey; i++) {
        for (int j = sx; j <= ex; j++) {
            cell[i][j] = ch;
        }
    }
}

int is_adjance(int y1, int x1, int y2, int x2) {
    if (y1 == y2 && (((x1 - x2) == 1) || ((x2 - x1) == 1))) return 1;
    if (x1 == x2 && (((y1 - y2) == 1) || ((y2 - y1) == 1))) return 1;
    return 0;
}

lvl* make_lvl() {
    lvl* lv = (lvl *)malloc(sizeof(lvl));
    lv->room_num = 6;
    lv->rooms = (room **)malloc(lv->room_num * sizeof(room *));
    lv->cell = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) 
        lv->cell[i] = (chtype *)malloc(col_lvl * sizeof(chtype));

    fill_inside(0, lv->cell, 0, 0, line_lvl, col_lvl);
    make_border(lv->cell, 0, 0, line_lvl, col_lvl);

    for (int i = 0; i < lv->room_num; i++) {
        int h, w, startx, starty;
        int ok = 0;
        do {
            h = get_rand(6, 10);
            w = get_rand(6, 25);
            starty = get_rand(2, line_lvl - h - 1);
            startx = get_rand(2, col_lvl - w - 1);
            ok = 1;
            for (int j = i - 1; j >= 0; --j) {
                if (over_lap(lv->rooms[j]->h, lv->rooms[j]->w, lv->rooms[j]->starty,
                                lv->rooms[j]->startx, h, w, starty, startx)) {
                    ok = 0;
                    break;
                }   
            }
        } while (!ok);
        int door_num = get_rand(2, 2);
        int pillar_num = get_rand(0, 2);
        lv->rooms[i] = make_room(h, w, door_num, pillar_num);
        lv->rooms[i]->starty = starty;
        lv->rooms[i]->startx = startx;
        fill_inside(el_floor, lv->cell, starty, startx, h, w);
        make_border(lv->cell, starty, startx, h, w);
        int sides[4] = {0};
        int rm_sd = 4;
        for (int j = 0; j < door_num; j++) {
            int ct = get_rand(1, rm_sd);
            int sd;
            for (sd = 0; ct > 0; sd++) {
                if (sides[sd] == 0) --ct; 
            }
            --sd;          
            sides[sd] = 1;
            --rm_sd;
            int y, x;
            /*
                0
            3       1
                2
            */
            if (sd == 0) {
                y = starty;
                x = startx + get_rand(1, w - 2);
            } else if (sd == 2) {
                y = starty + h - 1;
                x = startx + get_rand(1, w - 2);
            } else if (sd == 3 ) {
                y = starty + get_rand(1, h - 2);
                x = startx;
            } else {
                y = starty + get_rand(1, h - 2);
                x = startx + w - 1;
            }
            lv->rooms[i]->door[j].y = y;
            lv->rooms[i]->door[j].x = x;
            lv->cell[y][x] = el_door;
        }
        for (int j = 0; j < pillar_num; j++) {
            int y, x;
            ok = 0;
            do {
                y = starty + get_rand(1, h - 2);
                x = startx + get_rand(1, w - 2);
                ok = 1;
                for (int k = 0; k < door_num; k++) {
                    if (is_adjance(y, x, lv->rooms[i]->door[k].y, lv->rooms[i]->door[k].x)) {
                        ok = 0;
                        break;
                    }
                }
            } while(!ok);
            lv->rooms[i]->pillar[j].y = y;
            lv->rooms[i]->pillar[j].x = x;
            lv->cell[y][x] = el_pillar;
        }
    }

    return lv;
}

map* generate_map(user *player) {
    map *mp = (map *)malloc(sizeof(map));
    mp->lvl_num = 4;
    mp->lvls = (lvl **)malloc(mp->lvl_num * sizeof(lvl*));
    for (int i = 0; i < mp->lvl_num; i++) {
        mp->lvls[i] = make_lvl();
        if (i == 0) message_box_no_end("Map is generating.");
        if (i == 1) message_box_no_end("wait more");
        if (i == 2) message_box_no_end("it's almost done!");
    }
    return mp;
}

int main() {
    user *player = raw_user();
    initscr();
    keypad(stdscr, true);
    curs_set(0);
    noecho();
    start_color();
    set_colors();
    init_elmnts();
    refresh();
    // getch();
    message_box("start");
    message_box_no_end("Please wait, don't press any key!");
    map *new_map = generate_map(player);
    message_box("Map generated successfully, press any key to continue.");
    
    message_box("press to show lvl");
    for (int i = 0; i < new_map->lvl_num; i++) {
        lvl *lv = new_map->lvls[i];
        for (int i = 0; i < line_lvl; i++) {
            for (int j = 0; j < col_lvl; j++) {
                if (lv->cell[i][j] == 0) continue;
                mvaddch(starty_lvl + i, startx_lvl + j, lv->cell[i][j]);
            }
        }
        refresh();
        message_box("press to show next lvl");
        clear();
    }
    endwin();
    return 0;
}
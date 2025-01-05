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
chtype el_window;
chtype el_trap;
chtype el_stair;
chtype el_hidden_door;
chtype el_password_door; //locked : red; unlocked : green;
chtype el_password_maker;

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
    el_floor = '.' | COLOR_PAIR(GRAY_ON_BLACK) | A_REVERSE;
    el_road = '#' | COLOR_PAIR(GRAY_ON_BLACK) | A_REVERSE;
    el_window = '=' | COLOR_PAIR(RED_ON_BLACK) | A_REVERSE;
    el_trap = '^' | COLOR_PAIR(RED_ON_GRAY);
    el_stair = '<' | COLOR_PAIR(WHITE_ON_GRAY);
    el_hidden_door = '?' | COLOR_PAIR(GRAY_ON_BLACK) | A_REVERSE;
    el_password_door = '@' | COLOR_PAIR(RED_ON_GRAY);
    el_password_maker = '&' | COLOR_PAIR(YELLOW_ON_GRAY);
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
    int **room_id;
} lvl;

typedef struct map_type {
    int lvl_num;
    lvl** lvls;
} map;

int get_rand(int l, int r) {
    return l + (rand() % (r - l + 1));
}

room* make_room(int h, int w, int door_num) {
    room* rm = (room *)malloc(sizeof(room));
    rm->h = h;
    rm->w = w;
    rm->door_num = door_num;
    rm->door = (elmnt *)malloc(door_num * sizeof(elmnt));
    for (int i = 0; i < door_num; i++) {
        rm->door[i].y = -1;
        rm->door[i].x = -1;
    }
    return rm;
}

int is_wall(chtype ch) {
    if (ch == ul_corner) return 1;
    if (ch == ur_corner) return 1;
    if (ch == ll_corner) return 1;
    if (ch == lr_corner) return 1;
}

int is_adjance(int y1, int x1, int y2, int x2) {
    if (y1 == y2 && (((x1 - x2) == 1) || ((x2 - x1) == 1))) return 1;
    if (x1 == x2 && (((y1 - y2) == 1) || ((y2 - y1) == 1))) return 1;
    return 0;
}

int dont_cover_doors(room *rm, int y, int x) {
    for (int k = 0; k < rm->door_num; k++) {
        if (is_adjance(y, x, rm->door[k].y, rm->door[k].x)) {
            return 0;
        }
    }
    return 1;
}

int is_floor (lvl *lv, int y, int x) {
    if (lv->cell[y][x] == el_floor && lv->hidden_sit[y][x] == 0) return 1;
    return 0;
}

void add_cell(chtype ch, lvl *lv, int y, int x, int hidden_sit) {
    if (hidden_sit == 0) lv->cell[y][x] = ch;
    lv->hidden_sit[y][x] = hidden_sit;
    if (hidden_sit) lv->hidden_cell[y][x] = ch;
}

void set_el_in_room(chtype el_, lvl *lv, int rm_id, int cnt, int hidden_sit) {
    while (cnt--) {
        int starty = lv->rooms[rm_id]->starty;
        int startx = lv->rooms[rm_id]->startx;
        int h = lv->rooms[rm_id]->h;
        int w = lv->rooms[rm_id]->w;
        int y, x;
        int ok = 0;
        do {
            y = starty + get_rand(1, h - 2);
            x = startx + get_rand(1, w - 2);
            ok = dont_cover_doors(lv->rooms[rm_id], y, x) && is_floor(lv, y, x);
        } while(!ok);
        add_cell(el_, lv, y, x, hidden_sit);
    }
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

void bfs(lvl *lv, chtype **cell, elmnt src, elmnt dest, int **mark) {

    elmnt *queue = (elmnt *)malloc((line_lvl * col_lvl + 10) * sizeof(elmnt));
    elmnt **par = (elmnt **)malloc(line_lvl * sizeof(elmnt *));
    for (int i = 0; i < line_lvl; i++) {
        par[i] = (elmnt *)malloc(col_lvl * sizeof(elmnt));
        for (int j = 0; j < col_lvl; j++) {
            par[i][j].y = i;
            par[i][j].x = j;
        }
    }
    int frnt = 0;
    int bck = 0;
    queue[bck].y = src.y; queue[bck].x = src.x;
    mark[src.y][src.x] = 1;
    while (bck <= frnt) {
        int ykid, xkid;
        int ypar = queue[bck].y, xpar = queue[bck].x; ++bck;
        if (ypar == dest.y && xpar == dest.x) break;
        if (ypar > 0) {
            ykid = ypar - 1;
            xkid = xpar;
            if (!mark[ykid][xkid]) {
                mark[ykid][xkid] = 1;
                queue[++frnt].y = ykid;
                queue[frnt].x = xkid;
                par[ykid][xkid].y = ypar;
                par[ykid][xkid].x = xpar;
            }
        }
        if (xpar > 0) {
            ykid = ypar;
            xkid = xpar - 1;
            if (!mark[ykid][xkid]) {
                mark[ykid][xkid] = 1;
                queue[++frnt].y = ykid;
                queue[frnt].x = xkid;
                par[ykid][xkid].y = ypar;
                par[ykid][xkid].x = xpar;
            }
        }
        if (ypar < line_lvl - 1) {
            ykid = ypar + 1;
            xkid = xpar;
            if (!mark[ykid][xkid]) {
                mark[ykid][xkid] = 1;
                queue[++frnt].y = ykid;
                queue[frnt].x = xkid;
                par[ykid][xkid].y = ypar;
                par[ykid][xkid].x = xpar;
            }
        }
        if (xpar < col_lvl - 1) {
            ykid = ypar;
            xkid = xpar + 1;
            if (!mark[ykid][xkid]) {
                mark[ykid][xkid] = 1;
                queue[++frnt].y = ykid;
                queue[frnt].x = xkid;
                par[ykid][xkid].y = ypar;
                par[ykid][xkid].x = xpar;
            }
        }
    }
    elmnt temp;
    temp.y = par[dest.y][dest.x].y;
    temp.x = par[dest.y][dest.x].x;
    for (int i = 0; i <= frnt; i++) mark[queue[i].y][queue[i].x] = 0;
    while (par[temp.y][temp.x].y != temp.y || par[temp.y][temp.x].x != temp.x) {
        // cell[temp.y][temp.x] = el_road;
        add_cell(el_road, lv, temp.y, temp.x, 1);
        int y = temp.y;
        mark[temp.y][temp.x] = 1;
        temp.y = par[y][temp.x].y;
        temp.x = par[y][temp.x].x;
    }
    free(queue);
    for (int i = 0; i < line_lvl; i++) {
        free(par[i]);
    }
    free(par);
}

void make_path (lvl *lv, chtype **cell, elmnt src, elmnt dest, int diff) {
    int y, x;
    int ok = 0;
    do {
        y = get_rand(1, line_lvl - 2);
        x = get_rand(1, col_lvl - 2);
        if (cell[y][x] == 0 || cell[y][x] == el_road) ok = 1;
    } while(!ok);
    elmnt cnctor;
    cnctor.y = y; cnctor.x = x;
    int **mark = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        mark[i] = (int *)malloc(col_lvl * sizeof(int));
        for (int j = 0; j < col_lvl; j++) {
            if (cell[i][j] == el_road || cell[i][j] == 0) mark[i][j] = 0;
            else mark[i][j] = 1;
        }
    }

    if (diff == dif_hard) {
        mark[cnctor.y][cnctor.x] = 0;
        bfs(lv, cell, src, cnctor, mark);
        mark[dest.y][dest.x] = 0;
        bfs(lv, cell, cnctor, dest, mark);
        // cell[y][x] = el_road;
        add_cell(el_road, lv, y, x, 1);
    } else {
        mark[dest.y][dest.x] = 0;
        bfs(lv, cell, src, dest, mark);
    }
    for (int i = 0; i < line_lvl; i++) {
        free(mark[i]);
    }
    free(mark);
}

void make_roads (lvl *lv, int diff) {
    int *frnt = (int *)malloc(lv->room_num * sizeof(int));
    for (int i = 0; i < lv->room_num; i++) frnt[i] = 0;
    for (int i = 1; i < lv->room_num; i++) 
        make_path(lv, lv->cell, lv->rooms[i - 1]->door[frnt[i - 1]++], lv->rooms[i]->door[frnt[i]++], diff);
    for (int i = 0; i < lv->room_num; i++) {
        while(frnt[i] < lv->rooms[i]->door_num) {
            int j = get_rand(0, lv->room_num - 1);
            if (j == i) {
                if (i == 0) ++j;
                else --j;
            }
            if (frnt[j] < lv->rooms[j]->door_num)
                make_path(lv, lv->cell, lv->rooms[i]->door[frnt[i]++], lv->rooms[j]->door[frnt[j]++], diff);
            else {
                int k = get_rand(0, lv->rooms[j]->door_num - 1);
                make_path(lv, lv->cell, lv->rooms[i]->door[frnt[i]++], lv->rooms[j]->door[k], diff);
            }
        }
    }
    free(frnt);
}

lvl* make_lvl(int diff) {
    lvl* lv = (lvl *)malloc(sizeof(lvl));
    lv->room_num = 6;
    lv->rooms = (room **)malloc(lv->room_num * sizeof(room *));
    lv->cell = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) 
        lv->cell[i] = (chtype *)malloc(col_lvl * sizeof(chtype));

    lv->hidden_cell = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) 
        lv->hidden_cell[i] = (chtype *)malloc(col_lvl * sizeof(chtype));
    
    lv->room_id = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) 
        lv->room_id[i] = (int *)malloc(col_lvl * sizeof(int));
    
    lv->hidden_sit = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        lv->hidden_sit[i] = (int *)malloc(col_lvl * sizeof(int));
        for (int j = 0; j < col_lvl; j++)
            lv->hidden_sit[i][j] = 0;
    }

    fill_inside(0, lv->cell, 0, 0, line_lvl, col_lvl);
    fill_inside(0, lv->hidden_cell, 0, 0, line_lvl, col_lvl);
    make_border(lv->cell, 0, 0, line_lvl, col_lvl);

    for (int i = 0; i < lv->room_num; i++) {
        int h, w, startx, starty;
        int ok = 0;
        do {
            h = get_rand(6, 10);
            w = get_rand(6, 25);
            starty = get_rand(2, line_lvl - h - 2);
            startx = get_rand(2, col_lvl - w - 2);
            ok = 1;
            for (int j = i - 1; j >= 0; --j) {
                if (over_lap(lv->rooms[j]->h, lv->rooms[j]->w, lv->rooms[j]->starty,
                                lv->rooms[j]->startx, h, w, starty, startx)) {
                    ok = 0;
                    break;
                }   
            }
        } while (!ok);
        for (int ii = starty; ii < starty + h; ii++) {
            for (int j = startx; j < startx + w; j++) {
                lv->room_id[ii][j] = i;
            }
        }
        //door
        int door_num = get_rand(2, 2);
        lv->rooms[i] = make_room(h, w, door_num);
        lv->rooms[i]->starty = starty;
        lv->rooms[i]->startx = startx;
        fill_inside(el_floor, lv->cell, starty, startx, h, w);
        make_border(lv->cell, starty, startx, h, w);
        int sides[4] = {0};
        int rm_sd = 4;
        for (int j = 0; j < door_num; j++) {
            ok = 0;
            int y, x;
            do {
                int ct = get_rand(1, rm_sd);
                int sd;
                for (sd = 0; ct > 0; sd++) {
                    if (sides[sd] == 0) --ct; 
                }
                --sd;          
                sides[sd] = 1;
                --rm_sd;
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
                if (is_wall(lv->cell[y][x])) ok = 1;
            } while (!ok);
            lv->rooms[i]->door[j].y = y;
            lv->rooms[i]->door[j].x = x;
            lv->cell[y][x] = el_door;
        }
        //
        //pillar
        set_el_in_room(el_pillar, lv, i, get_rand(0, diff), 0);
        //
        //window
        if (get_rand(0, 1) == 0) {
            int y, x;
            int ok = 0;
            do {
                int sd = get_rand(0, 3);
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
                if (is_wall(lv->cell[y][x])) ok = 1;
            } while (!ok);
            lv->cell[y][x] = el_window;
        }
        //
        //trap
        set_el_in_room(el_trap, lv, i, get_rand(0, diff), 1);
        //
    }
    //stair
    set_el_in_room(el_stair, lv, get_rand(1, lv->room_num - 1), 1, 0);
    //
    //hidden_door
    set_el_in_room(el_hidden_door, lv, get_rand(0, lv->room_num - 1), 1, 1);
    //
    //password_door and password_maker
    int temp;
    set_el_in_room(el_password_door, lv, temp = get_rand(0, lv->room_num - 1), 1, 0);
    set_el_in_room(el_password_maker, lv, temp, 1, 0);
    //
    make_roads(lv, diff);
    return lv;
}

map* generate_map(user *player) {
    map *mp = (map *)malloc(sizeof(map));
    mp->lvl_num = 4;
    mp->lvls = (lvl **)malloc(mp->lvl_num * sizeof(lvl*));
    for (int i = 0; i < mp->lvl_num; i++) {
        mp->lvls[i] = make_lvl(player->difficulty);
        // if (i == 0) message_box("Map is generating.");
        // if (i == 1) message_box_no_end("wait more");
        // if (i == 2) message_box_no_end("it's almost done!");
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
                if (lv->cell[i][j] == 0 && lv->hidden_sit[i][j] == 0) continue;
                if (lv->hidden_sit[i][j]) mvaddch(starty_lvl + i, startx_lvl + j, lv->hidden_cell[i][j]); 
                else mvaddch(starty_lvl + i, startx_lvl + j, lv->cell[i][j]);
            }
        }
        refresh();
        message_box("press to show next lvl");
        clear();
    }
    endwin();
    return 0;
}
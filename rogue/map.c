#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>

#include "menu.h"

int line_lvl;
int col_lvl;
int starty_lvl;
int startx_lvl;
int delta_y;
int delta_x;
chtype h_line;
chtype v_line;
chtype ul_corner;
chtype ur_corner;
chtype ll_corner;
chtype lr_corner;
chtype el_door;
chtype el_floor;
chtype el_road;
chtype el_window;
chtype el_trap;
chtype el_stair;
chtype el_stair_up;
chtype el_hidden_door;

char *gold_regular;
char *gold_black; //each 5 time reg;
chtype gold_regular_attr;
chtype gold_black_attr;

char *talisman_health;
char *talisman_speed;
char *talisman_damage;
chtype talisman_health_attr;
chtype talisman_speed_attr;
chtype talisman_damage_attr;

char *weapon_sword;
char *weapon_mace; //gorz
char *weapon_dagger; //khankar
char *weapon_magic_wand;
char *weapon_arrow;
chtype weapon_sword_attr;
chtype weapon_mace_attr;
chtype weapon_dagger_attr;
chtype weapon_magic_wand_attr;
chtype weapon_arrow_attr;

char *food_reg;
char *food_golden; //aala
char *food_magical;
char *food_rotten;
chtype food_reg_attr;
chtype food_golden_attr;
chtype food_magical_attr;
chtype food_rotten_attr;

char *hero;
chtype hero_attr;

void init_elmnts () {
    srand(time(0));
    remnant = NULL;
    delta_y = 5;
    delta_x = 40;
    line_lvl = LINES - delta_y;
    col_lvl = COLS - delta_x;
    starty_lvl = delta_y;
    startx_lvl = delta_x;

    h_line = ACS_HLINE | COLOR_PAIR(RED_ON_BLACK);
    v_line = ACS_VLINE | COLOR_PAIR(RED_ON_BLACK);
    ul_corner = ACS_ULCORNER | COLOR_PAIR(RED_ON_BLACK);
    ur_corner = ACS_URCORNER | COLOR_PAIR(RED_ON_BLACK);
    ll_corner = ACS_LLCORNER | COLOR_PAIR(RED_ON_BLACK);
    lr_corner = ACS_LRCORNER | COLOR_PAIR(RED_ON_BLACK);
    el_door = '+' | COLOR_PAIR(GREEN_ON_BLACK);
    el_floor = '.' | COLOR_PAIR(GRAY_ON_BLACK);
    el_road = '#' | COLOR_PAIR(GRAY_ON_BLACK) | A_REVERSE;
    el_window = '=' | COLOR_PAIR(RED_ON_BLACK) ;
    el_trap = '^' | COLOR_PAIR(RED_ON_BLACK);
    el_stair = '>' | COLOR_PAIR(WHITE_ON_BLACK);
    el_stair_up = '<' | COLOR_PAIR(WHITE_ON_BLACK);
    el_hidden_door = '?' | COLOR_PAIR(GREEN_ON_BLACK);

    gold_regular = "*";
    gold_black = "*"; //each 5 time reg;
    gold_regular_attr = COLOR_PAIR(YELLOW_ON_BLACK);
    gold_black_attr = COLOR_PAIR(PURPLE_D_ON_BLACK);

    talisman_health = "⚕"; //"\u2695";
    talisman_speed = "⌁"; //"\u2301";
    talisman_damage = "ᛟ";
    talisman_health_attr = COLOR_PAIR(GREEN_ON_BLACK);
    talisman_speed_attr = COLOR_PAIR(YELLOW_ON_BLACK);
    talisman_damage_attr = COLOR_PAIR(RED_ON_BLACK);

    weapon_sword = "⚸"; //"\u26B8";
    weapon_mace = "⚵"; //"\u26B5"; //gorz
    weapon_dagger = "࿈"; //khankar
    weapon_magic_wand = "☨"; //"\u2628";
    weapon_arrow = "↢"; //"\u21A2";
    weapon_sword_attr = COLOR_PAIR(RED_ON_BLACK);
    weapon_mace_attr = COLOR_PAIR(RED_ON_BLACK);
    weapon_dagger_attr = COLOR_PAIR(GRAY_ON_BLACK);
    weapon_magic_wand_attr = COLOR_PAIR(PURPLE_D_ON_BLACK);
    weapon_arrow_attr = COLOR_PAIR(GREEN_ON_BLACK);

    food_reg = "ၜ";
    food_golden = "ၜ"; //aala
    food_magical = "ၜ";
    food_rotten = "ၜ";
    food_reg_attr = COLOR_PAIR(GREEN_ON_BLACK);
    food_golden_attr = COLOR_PAIR(YELLOW_ON_BLACK);
    food_magical_attr = COLOR_PAIR(PURPLE_D_ON_BLACK);
    food_rotten_attr = COLOR_PAIR(RED_ON_BLACK);

    hero = "⍢"; //"\u2362";
    hero_attr = A_BOLD | COLOR_PAIR(WHITE_ON_BLACK);

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
    // elmnt wndow;
    // elmnt trap;
    // elmnt stair;
    // elmnt hidden_door;
} room;

typedef struct pickable_things_type {
    int **pickable_sit; //0 nothing; 1 pickable; 2 ilusion;
    char ***cells;
    chtype **attr;
    int **cnt;
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
    chtype **cell;
    chtype **hidden_cell;

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
    if ((ch & ACS_ULCORNER) == ACS_ULCORNER) return 1;
    if ((ch & ACS_URCORNER) == ACS_URCORNER) return 1;
    if ((ch & ACS_LLCORNER) == ACS_LLCORNER) return 1;
    if ((ch & ACS_LRCORNER) == ACS_LRCORNER) return 1;
    if ((ch & ACS_HLINE) == ACS_HLINE) return 1;
    if ((ch & ACS_VLINE) == ACS_VLINE) return 1;
    return 0;
}

int is_this_floor(chtype ch) {
    if ((ch & '.') == '.') return 1;
    return 0;
}

int is_filled(lvl *lv, int y, int x) {
    if (!is_this_floor(lv->cell[y][x])) return 1;
    if (lv->hidden_sit[y][x]) return 1;
    if (lv->golds->pickable_sit[y][x]) return 1;
    if (lv->weapons->pickable_sit[y][x]) return 1;
    if (lv->talismans->pickable_sit[y][x]) return 1;
    if (lv->foods->pickable_sit[y][x]) return 1;
    return 0;
}

int is_adjance(int y1, int x1, int y2, int x2) {
    int h = y1 - y2;
    int w = x1 - x2;
    if (h < 0) h *= -1;
    if (w < 0) w *= -1;
    if ((h + w) == 1) return 1;
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
    if (is_this_floor(lv->cell[y][x]) && lv->hidden_sit[y][x] == 0) return 1;
    return 0;
}

void add_cell(chtype ch, lvl *lv, int y, int x, int hidden_sit) {
    if (hidden_sit == 0) lv->cell[y][x] = ch;
    else lv->hidden_cell[y][x] = ch;
    lv->hidden_sit[y][x] = hidden_sit;
    
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

elmnt * give_set_el_in_room(lvl *lv, int rm_id) {
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
    elmnt *res = (elmnt *)malloc(sizeof(elmnt));
    res->y = y;
    res->x = x;
    return res;
}

elmnt * give_free_elmnt_in_room (lvl *lv, int rm_id) {
    int starty = lv->rooms[rm_id]->starty;
    int startx = lv->rooms[rm_id]->startx;
    int h = lv->rooms[rm_id]->h;
    int w = lv->rooms[rm_id]->w;
    int y, x;
    int ok = 0;
    int c = 0;
    do {
        y = starty + get_rand(1, h - 2);
        x = startx + get_rand(1, w - 2);
        ok = dont_cover_doors(lv->rooms[rm_id], y, x) && !is_filled(lv, y, x);
    } while(!ok);
    elmnt *res = (elmnt *)malloc(sizeof(elmnt));
    res->y = y;
    res->x = x;
    return res;
}


elmnt * give_free_elmnt_in_room_monster (lvl *lv, int rm_id) {
    int starty = lv->rooms[rm_id]->starty;
    int startx = lv->rooms[rm_id]->startx;
    int h = lv->rooms[rm_id]->h;
    int w = lv->rooms[rm_id]->w;
    int y, x;
    int ok = 0;
    int c = 0;
    do {
        y = starty + get_rand(1, h - 2);
        x = startx + get_rand(1, w - 2);
        int ok2 = 1;
        for (int i = 0; i < lv->monster_num; i++) {
            if (lv->monster[i]->cnt[y][x]) ok2 = 0;
        }
        ok = dont_cover_doors(lv->rooms[rm_id], y, x) && !is_filled(lv, y, x) && ok2;
    } while(!ok);
    elmnt *res = (elmnt *)malloc(sizeof(elmnt));
    res->y = y;
    res->x = x;
    return res;
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

void make_border(chtype atr, chtype **cell, int sy, int sx, int h, int w) {
    int ey = sy + h - 1;
    int ex = sx + w - 1;
    cell[sy][sx] = ACS_ULCORNER | atr;
    cell[sy][ex] = ACS_URCORNER | atr;
    cell[ey][sx] = ACS_LLCORNER | atr;
    cell[ey][ex] = ACS_LRCORNER | atr;
    for (int i = sy + 1; i < ey; i++) {
        cell[i][sx] = ACS_VLINE | atr;
        cell[i][ex] = ACS_VLINE | atr;
    }
    for (int i = sx + 1; i < ex; i++) {
        cell[sy][i] = ACS_HLINE | atr;
        cell[ey][i] = ACS_HLINE | atr;
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

void fill_inside_int(int ch, int **arr, int sy, int sx, int h, int w) {
    int ey = sy + h - 1;
    int ex = sx + w - 1;
    for (int i = sy; i <= ey; i++) {
        for (int j = sx; j <= ex; j++) {
            arr[i][j] = ch;
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
        add_cell(el_road, lv, temp.y, temp.x, 0);
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
        add_cell(el_road, lv, y, x, 0);
    } else {
        mark[dest.y][dest.x] = 0;
        bfs(lv, cell, src, dest, mark);
    }
    for (int i = 0; i < line_lvl; i++) {
        free(mark[i]);
    }
    free(mark);
}

void make_roads (lvl *lv, int diff, int reg_room_num) {
    int *frnt = (int *)malloc(reg_room_num * sizeof(int));
    for (int i = 0; i < reg_room_num; i++) frnt[i] = 0;
    for (int i = 1; i < reg_room_num; i++) 
        make_path(lv, lv->cell, lv->rooms[i - 1]->door[frnt[i - 1]++], lv->rooms[i]->door[frnt[i]++], diff);
    for (int i = 0; i < reg_room_num; i++) {
        while(frnt[i] < lv->rooms[i]->door_num) {
            int j = get_rand(0, reg_room_num - 1);
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

lvl* make_lvl(int diff, int is_first_lvl, int is_last_lvl) {
    lvl* lv = (lvl *)malloc(sizeof(lvl));
    lv->reg_room_num = 6;
    lv->room_num = lv->reg_room_num;

    lv->nightmare_room_id = get_rand(0, 1);
    if (lv->nightmare_room_id) {
        lv->nightmare_room_id = lv->room_num;
        ++(lv->room_num);
    } else lv->nightmare_room_id = 5001;

    lv->enchant_room_id = get_rand(0, 1);
    if (lv->enchant_room_id) {
        lv->enchant_room_id = lv->room_num;
        ++(lv->room_num);
    } else lv->enchant_room_id = 5002;

    lv->treasure_room_id = is_last_lvl;
    if (lv->treasure_room_id) {
        lv->treasure_room_id = lv->room_num;
        ++(lv->room_num);
    } else lv->treasure_room_id = 5003;

    lv->rooms = (room **)malloc(lv->room_num * sizeof(room *));
    lv->cell = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) 
        lv->cell[i] = (chtype *)malloc(col_lvl * sizeof(chtype));

    lv->hidden_cell = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) 
        lv->hidden_cell[i] = (chtype *)malloc(col_lvl * sizeof(chtype));

    lv->room_id = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        lv->room_id[i] = (int *)malloc(col_lvl * sizeof(int));
        for (int j = 0; j < col_lvl; j++) {
            lv->room_id[i][j] = -1;
        }
    }
    
    lv->hidden_sit = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        lv->hidden_sit[i] = (int *)malloc(col_lvl * sizeof(int));
        for (int j = 0; j < col_lvl; j++)
            lv->hidden_sit[i][j] = 0;
    }

    lv->explore_sit = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        lv->explore_sit[i] = (int *)malloc(col_lvl * sizeof(int));
        for (int j = 0; j < col_lvl; j++)
            lv->explore_sit[i][j] = 0;
    }

    fill_inside(0, lv->cell, 0, 0, line_lvl, col_lvl);
    fill_inside(0, lv->hidden_cell, 0, 0, line_lvl, col_lvl);
    make_border(h_line ^ ACS_HLINE ^ COLOR_PAIR(RED_ON_BLACK) | COLOR_PAIR(BLUE_ON_BLACK), lv->cell, 0, 0, line_lvl, col_lvl);

    //rooms
    for (int i = 0; i < lv->room_num; i++) {
        int h, w, startx, starty;
        int ok = 0;
        do {
            h = get_rand(9, 12);
            w = get_rand(9, 30);
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
        if (i >= lv->reg_room_num) door_num = 1;
        lv->rooms[i] = make_room(h, w, door_num);
        lv->rooms[i]->starty = starty;
        lv->rooms[i]->startx = startx;

        if (i == lv->nightmare_room_id) {
            chtype atr = COLOR_PAIR(PURPLE_D_ON_BLACK) | A_BOLD;
            fill_inside('.' | COLOR_PAIR(PURPLE_D_ON_BLACK), lv->cell, starty, startx, h, w);
            make_border(atr,lv->cell, starty, startx, h, w);
        } else if (i == lv->treasure_room_id) {
            chtype atr = COLOR_PAIR(YELLOW_ON_BLACK) | A_BOLD;
            fill_inside('.' | COLOR_PAIR(WHITE_ON_BLACK), lv->cell, starty, startx, h, w);
            make_border(atr,lv->cell, starty, startx, h, w);
        } else if (i == lv->enchant_room_id) {
            chtype atr = COLOR_PAIR(BLUE_D_ON_BLACK) | A_BOLD;
            fill_inside('.' | COLOR_PAIR(BLUE_D_ON_BLACK), lv->cell, starty, startx, h, w);
            make_border(atr,lv->cell, starty, startx, h, w);
        } else {
            fill_inside(el_floor, lv->cell, starty, startx, h, w);
            make_border(h_line ^ ACS_HLINE | A_BOLD,lv->cell, starty, startx, h, w);
        }

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
            if (i >= lv->reg_room_num) {
                add_cell(el_hidden_door, lv, y, x, 1);
            } else {
                lv->cell[y][x] = el_door;
            }
        }
       
        
    }

    //special rooms
    for (int i = lv->reg_room_num; i < lv->room_num; i++) {
        int par = get_rand(0, lv->reg_room_num - 1);
        int h = lv->rooms[par]->h;
        int w = lv->rooms[par]->w;
        int startx = lv->rooms[par]->startx;
        int starty = lv->rooms[par]->starty;
        int ok = 0;
        ok = 0;
        int y, x;
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
        elmnt src;
        src.x = lv->rooms[i]->door[0].x;
        src.y = lv->rooms[i]->door[0].y;

        elmnt dest;
        dest.x = x;
        dest.y = y;
        make_path(lv, lv->cell, src, dest, diff);
        add_cell(el_hidden_door, lv, y, x, 1);
    }

    //window and traps
    for (int i = 0; i < lv->reg_room_num; i++) {
        //window
        int h = lv->rooms[i]->h;
        int w = lv->rooms[i]->w;
        int startx = lv->rooms[i]->startx;
        int starty = lv->rooms[i]->starty;
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
        set_el_in_room(el_trap, lv, i, get_rand(0, diff), 0);
        //
    }
    
    //stair
    
    
    elmnt *stair_up;
    stair_up = give_set_el_in_room(lv, 0);
    lv->up_stair.y = stair_up->y;
    lv->up_stair.x = stair_up->x;
    if (!is_first_lvl) add_cell(el_stair_up, lv, stair_up->y, stair_up->x, 0);
    
    if (!is_last_lvl) {
        elmnt *stair;
        int rm_id = get_rand(1, lv->reg_room_num - 1);
        stair = give_set_el_in_room(lv, rm_id);
        lv->down_stair.y = stair->y;
        lv->down_stair.x = stair->x;
        add_cell(el_stair, lv, stair->y, stair->x, 0);
    }
    
    //roads
    make_roads(lv, diff, lv->reg_room_num);

    return lv;
}

void add_monsters(lvl *lv, int diff) {
    //monster
    lv->monster_num = 5;
    lv->monster = (enemy **)malloc(lv->monster_num * sizeof(enemy *));
    for (int i = 0; i < lv->monster_num; i++) {
        lv->monster[i] = (enemy *)malloc(sizeof(enemy));
        lv->monster[i]->cnt = (int **)malloc(line_lvl * sizeof(int *));
        for (int j = 0; j < line_lvl; j++) {
            lv->monster[i]->cnt[j] = (int *)malloc(col_lvl * sizeof(int));
            for (int k = 0; k < col_lvl; k++) {
                lv->monster[i]->cnt[j][k] = 0;
            }
        }
        lv->monster[i]->hp = (int **)malloc(line_lvl * sizeof(int *));
        for (int j = 0; j < line_lvl; j++) {
            lv->monster[i]->hp[j] = (int *)malloc(col_lvl * sizeof(int));
            for (int k = 0; k < col_lvl; k++) {
                lv->monster[i]->hp[j][k] = 0;
            }
        }
        lv->monster[i]->token = (int **)malloc(line_lvl * sizeof(int *));
        for (int j = 0; j < line_lvl; j++) {
            lv->monster[i]->token[j] = (int *)malloc(col_lvl * sizeof(int));
            for (int k = 0; k < col_lvl; k++) {
                lv->monster[i]->token[j][k] = 0;
            }
        }
    }
    for (int i = 0; i < lv->monster_num; i++) {
        enemy *mon = lv->monster[i];
        int c;
        switch (i) {
            case 0 : 
                mon->look = 'D';
                mon->name = "Deamon";
                mon->max_hp = 5;
                mon->max_token = 0;
                mon->damage = 10;
                c = 67;
                init_pair(c, c, BLACK);
                mon->attr = A_BOLD | COLOR_PAIR(c); 
                break;
            case 1 : 
                mon->look = 'F'; 
                mon->name = "Fire breathing monster"; 
                mon->max_hp = 10;
                mon->max_token = 0;
                mon->damage = 20;
                c = 220;
                init_pair(c, c, BLACK);
                mon->attr = A_BOLD | COLOR_PAIR(c); 
                break;
            case 2 : 
                mon->look = 'G'; 
                mon->name = "Giant"; 
                mon->max_hp = 15;
                mon->max_token = 5;
                mon->damage = 15;
                c = 174;
                init_pair(c, c, BLACK);
                mon->attr = A_BOLD | COLOR_PAIR(c); 
                break;
            case 3 : 
                mon->look = 'S'; 
                mon->name = "Snake";
                mon->max_hp = 20;
                mon->max_token = 1000;
                mon->damage = 15; 
                c = 208;
                init_pair(c, c, BLACK);
                mon->attr = A_BOLD | COLOR_PAIR(c); 
                break;
            case 4 : 
                mon->look = 'U'; 
                mon->name = "Undead"; 
                mon->max_hp = 30;
                mon->max_token = 5;
                mon->damage = 25;
                c = 196;
                init_pair(c, c, BLACK);
                mon->attr = A_BOLD | COLOR_PAIR(c); 
                break;
            default : break;
        }
        for (int j = 0; j < lv->room_num; j++) {
            int num;
            if (j < lv->reg_room_num) {
                switch(mon->look) {
                    case 'D':
                        num = get_rand(0, 2 + diff - 1);
                        break;
                    case 'F':
                        num = get_rand(0, 2 + diff - 1);
                        break;
                    case 'G':
                        num = get_rand(0, 2 + diff - 1);
                        break;
                    case 'S':
                        num = get_rand(0, 1 + diff - 1);
                        break;
                    case 'U':
                        num = get_rand(0, 1 + diff - 1);
                        break;
                    default : break;
                }
            } else if (j == lv->treasure_room_id) {
                switch(mon->look) {
                    case 'D':
                        num = get_rand(1, 2 + diff - 1);
                        break;
                    case 'F':
                        num = get_rand(1, 2 + diff - 1);
                        break;
                    case 'G':
                        num = get_rand(1, 2 + diff - 1);
                        break;
                    case 'S':
                        num = get_rand(1, 2 + diff - 1);
                        break;
                    case 'U':
                        num = get_rand(1, 2 + diff - 1);
                        break;
                    default : break;
                }
            } else if (j == lv->nightmare_room_id) {
                switch(mon->look) {
                    case 'D':
                        num = get_rand(3, 4 + diff - 1);
                        break;
                    case 'F':
                        num = get_rand(3, 4 + diff - 1);
                        break;
                    case 'G':
                        num = get_rand(2, 3 + diff - 1);
                        break;
                    case 'S':
                        num = get_rand(1, 3 + diff - 1);
                        break;
                    case 'U':
                        num = get_rand(1, 3 + diff - 1);
                        break;
                    default : break;
                }
            } else num = 0; //enchant room;
              
            while (num--) {
                elmnt *temp;
                temp = give_free_elmnt_in_room_monster(lv, j);
                mon->cnt[temp->y][temp->x] += 1;
                if (mon->look == 'S') mon->token[temp->y][temp->x] = 1000;
                mon->hp[temp->y][temp->x] = mon->max_hp;
            }
        }
    }
}

void add_pickable_things (int diff, lvl *lv) {
    
    //gold init
    pickable_things *golds = (pickable_things *)malloc(sizeof(pickable_things));

    golds->pickable_sit = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        golds->pickable_sit[i] = (int *)malloc(col_lvl * sizeof(int));
    }

    golds->cnt = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        golds->cnt[i] = (int *)malloc(col_lvl * sizeof(int));
    }

    golds->attr = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) {
        golds->attr[i] = (chtype *)malloc(col_lvl * sizeof(chtype));
    }

    golds->cells = (char ***)malloc(line_lvl * sizeof(char **));
    for (int i = 0; i < line_lvl; i++) {
        golds->cells[i] = (char **)malloc(col_lvl * sizeof(char *));
        for (int j = 0; j < col_lvl; j++) {
            golds->cells[i][j] = (char *)malloc(50 * sizeof(char));
            golds->cells[i][j][0] = '\0';
        }
    }
    fill_inside_int(0, golds->pickable_sit, 0, 0, line_lvl, col_lvl);
    fill_inside_int(0, golds->cnt, 0, 0, line_lvl, col_lvl);
    fill_inside(0, golds->attr, 0, 0, line_lvl, col_lvl);

    //talismans init
    pickable_things *talisman = (pickable_things *)malloc(sizeof(pickable_things));

    talisman->pickable_sit = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        talisman->pickable_sit[i] = (int *)malloc(col_lvl * sizeof(int));
    }

    talisman->cnt = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        talisman->cnt[i] = (int *)malloc(col_lvl * sizeof(int));
    }

    talisman->attr = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) {
        talisman->attr[i] = (chtype *)malloc(col_lvl * sizeof(chtype));
    }

    talisman->cells = (char ***)malloc(line_lvl * sizeof(char **));
    for (int i = 0; i < line_lvl; i++) {
        talisman->cells[i] = (char **)malloc(col_lvl * sizeof(char *));
        for (int j = 0; j < col_lvl; j++) {
            talisman->cells[i][j] = (char *)malloc(50 * sizeof(char));
            talisman->cells[i][j][0] = '\0';
        }
    }

    fill_inside_int(0, talisman->pickable_sit, 0, 0, line_lvl, col_lvl);
    fill_inside_int(0, talisman->cnt, 0, 0, line_lvl, col_lvl);
    fill_inside(0, talisman->attr, 0, 0, line_lvl, col_lvl);

    //foods init
    pickable_things *foods = (pickable_things *)malloc(sizeof(pickable_things));

    foods->pickable_sit = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        foods->pickable_sit[i] = (int *)malloc(col_lvl * sizeof(int));
    }

    foods->cnt = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        foods->cnt[i] = (int *)malloc(col_lvl * sizeof(int));
    }

    foods->attr = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) {
        foods->attr[i] = (chtype *)malloc(col_lvl * sizeof(chtype));
    }

    foods->cells = (char ***)malloc(line_lvl * sizeof(char **));
    for (int i = 0; i < line_lvl; i++) {
        foods->cells[i] = (char **)malloc(col_lvl * sizeof(char *));
        for (int j = 0; j < col_lvl; j++) {
            foods->cells[i][j] = (char *)malloc(50 * sizeof(char));
            foods->cells[i][j][0] = '\0';
        }
    }

    fill_inside_int(0, foods->pickable_sit, 0, 0, line_lvl, col_lvl);
    fill_inside_int(0, foods->cnt, 0, 0, line_lvl, col_lvl);
    fill_inside(0, foods->attr, 0, 0, line_lvl, col_lvl);
    
    //weapons init
    pickable_things *weapons = (pickable_things *)malloc(sizeof(pickable_things));

    weapons->pickable_sit = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        weapons->pickable_sit[i] = (int *)malloc(col_lvl * sizeof(int));
    }

    weapons->cnt = (int **)malloc(line_lvl * sizeof(int *));
    for (int i = 0; i < line_lvl; i++) {
        weapons->cnt[i] = (int *)malloc(col_lvl * sizeof(int));
    }

    weapons->attr = (chtype **)malloc(line_lvl * sizeof(chtype *));
    for (int i = 0; i < line_lvl; i++) {
        weapons->attr[i] = (chtype *)malloc(col_lvl * sizeof(chtype));
    }

    weapons->cells = (char ***)malloc(line_lvl * sizeof(char **));
    for (int i = 0; i < line_lvl; i++) {
        weapons->cells[i] = (char **)malloc(col_lvl * sizeof(char *));
        for (int j = 0; j < col_lvl; j++) {
            weapons->cells[i][j] = (char *)malloc(50 * sizeof(char));
            weapons->cells[i][j][0] = '\0';
        }
    }

    fill_inside_int(0, weapons->pickable_sit, 0, 0, line_lvl, col_lvl);
    fill_inside_int(0, weapons->cnt, 0, 0, line_lvl, col_lvl);
    fill_inside(0, weapons->attr, 0, 0, line_lvl, col_lvl);
    
    lv->golds = golds;
    lv->talismans = talisman;
    lv->foods = foods;
    lv->weapons = weapons; 

    //gold
    for (int i = 0; i < lv->room_num; i++) {
        chtype reg_attr = gold_regular_attr;
        chtype black_attr = gold_black_attr;
        char *unie = "o";
        if (i == lv->nightmare_room_id) {
            int reg_cnt = get_rand(0, 3);
            int black_cnt = get_rand(0, 2);
            while (reg_cnt--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                golds->pickable_sit[y][x] = 1;
                golds->cnt[y][x] = 0;
                golds->attr[y][x] = reg_attr;
                strcpy(golds->cells[y][x], gold_regular);
            }
            while (black_cnt--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                golds->pickable_sit[y][x] = 1;
                golds->cnt[y][x] = 0;
                golds->attr[y][x] = black_attr;
                strcpy(golds->cells[y][x], gold_black);
            }
        } else if(i == lv->treasure_room_id) {
            int reg_cnt = get_rand(7, 10);
            int black_cnt = get_rand(3, 4);
            while (reg_cnt--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                golds->pickable_sit[y][x] = 1;
                golds->cnt[y][x] = 10;
                golds->attr[y][x] = reg_attr;
                strcpy(golds->cells[y][x], gold_regular);
            }
            while (black_cnt--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                golds->pickable_sit[y][x] = 1;
                golds->cnt[y][x] = 5;
                golds->attr[y][x] = black_attr;
                strcpy(golds->cells[y][x], gold_black);
            }
        } else if (i == lv->enchant_room_id) {
            int reg_cnt = get_rand(0, 2);
            int black_cnt = get_rand(0, 1);
            while (reg_cnt--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                golds->pickable_sit[y][x] = 1;
                golds->cnt[y][x] = get_rand(2, 5);
                golds->attr[y][x] = reg_attr;
                strcpy(golds->cells[y][x], gold_regular);
            }
            while (black_cnt--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                golds->pickable_sit[y][x] = 1;
                golds->cnt[y][x] = get_rand(1, 2);
                golds->attr[y][x] = black_attr;
                strcpy(golds->cells[y][x], gold_black);
            }
        } else {
            int reg_cnt = get_rand(1, 3);
            int black_cnt = get_rand(0, 2);
            while (reg_cnt--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                golds->pickable_sit[y][x] = 1;
                golds->cnt[y][x] = get_rand(2, 7);
                golds->attr[y][x] = reg_attr;
                strcpy(golds->cells[y][x], gold_regular);
            }
            while (black_cnt--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                golds->pickable_sit[y][x] = 1;
                golds->cnt[y][x] = get_rand(1, 3);
                golds->attr[y][x] = black_attr;
                strcpy(golds->cells[y][x], gold_black);
            }
        }
    }

    //talisman
    for (int i = 0; i < lv->room_num; i++) {
        
        if (i == lv->nightmare_room_id) {
            int health = get_rand(0, 1);
            int damage = get_rand(0, 2);
            int speed = get_rand(0, 1);
            while (health--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 0;
                talisman->attr[y][x] = talisman_health_attr;
                strcpy(talisman->cells[y][x], talisman_health);
            }   
            while (speed--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 0;
                talisman->attr[y][x] = talisman_speed_attr;
                strcpy(talisman->cells[y][x], talisman_speed);
            } 
            while (damage--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 0;
                talisman->attr[y][x] = talisman_damage_attr;
                strcpy(talisman->cells[y][x], talisman_damage);
            }
        } else if(i == lv->treasure_room_id) {
            int health = get_rand(0, 1);
            int damage = get_rand(0, 0);
            int speed = get_rand(0, 0);
            while (health--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_health_attr;
                strcpy(talisman->cells[y][x], talisman_health);
            }   
            while (speed--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_speed_attr;
                strcpy(talisman->cells[y][x], talisman_speed);
            } 
            while (damage--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_damage_attr;
                strcpy(talisman->cells[y][x], talisman_damage);
            }
        } else if (i == lv->enchant_room_id) {
            int health = get_rand(1, 4);
            int damage = get_rand(1, 4);
            int speed = get_rand(1, 4);
            while (health--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_health_attr;
                strcpy(talisman->cells[y][x], talisman_health);
            }   
            while (speed--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_speed_attr;
                strcpy(talisman->cells[y][x], talisman_speed);
            } 
            while (damage--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_damage_attr;
                strcpy(talisman->cells[y][x], talisman_damage);
            }
        } else {
            int health = get_rand(0, 1);
            int damage = get_rand(0, 1);
            int speed = get_rand(0, 1);
            while (health--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_health_attr;
                strcpy(talisman->cells[y][x], talisman_health);
            }   
            while (speed--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_speed_attr;
                strcpy(talisman->cells[y][x], talisman_speed);
            } 
            while (damage--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                talisman->pickable_sit[y][x] = 1;
                talisman->cnt[y][x] = 1;
                talisman->attr[y][x] = talisman_damage_attr;
                strcpy(talisman->cells[y][x], talisman_damage);
            }
        }
    }

    //food
    for (int i = 0; i < lv->room_num; i++) {
        
        if (i == lv->nightmare_room_id) {
            int reg = get_rand(0, 1);
            int golden = get_rand(0, 1);
            int magical = get_rand(0, 1);
            int rotten = get_rand(0, 1);
            while (reg--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 0;
                foods->attr[y][x] = food_reg_attr;
                strcpy(foods->cells[y][x], food_reg);
            }   
            while (golden--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 0;
                foods->attr[y][x] = food_golden_attr;
                strcpy(foods->cells[y][x], food_golden);
            } 
            while (magical--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 0;
                foods->attr[y][x] = food_magical_attr;
                strcpy(foods->cells[y][x], food_magical);
            }
            while (rotten--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 0;
                foods->attr[y][x] = food_rotten_attr;
                strcpy(foods->cells[y][x], food_rotten);
            }
        } else if(i == lv->treasure_room_id) {
            int reg = get_rand(0, 1);
            int golden = get_rand(1, 2);
            int magical = get_rand(0, 1);
            int rotten = get_rand(0, 0);
            while (reg--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_reg_attr;
                strcpy(foods->cells[y][x], food_reg);
            }   
            while (golden--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_golden_attr;
                strcpy(foods->cells[y][x], food_golden);
            } 
            while (magical--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_magical_attr;
                strcpy(foods->cells[y][x], food_magical);
            }
            while (rotten--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_rotten_attr;
                strcpy(foods->cells[y][x], food_rotten);
            }
        } else if (i == lv->enchant_room_id) {
            int reg = get_rand(0, 1);
            int golden = get_rand(0, 1);
            int magical = get_rand(1, 2);
            int rotten = get_rand(0, 1);
            while (reg--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_reg_attr;
                strcpy(foods->cells[y][x], food_reg);
            }   
            while (golden--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_golden_attr;
                strcpy(foods->cells[y][x], food_golden);
            } 
            while (magical--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_magical_attr;
                strcpy(foods->cells[y][x], food_magical);
            }
            while (rotten--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_rotten_attr;
                strcpy(foods->cells[y][x], food_rotten);
            }
        } else {
            int reg = get_rand(0, 2);
            int golden = get_rand(0, 2);
            if (golden) golden = 0;
            golden = 1;
            int magical = get_rand(0, 2);
            if (magical) magical = 0;
            magical = 1;
            int rotten = get_rand(0, 1);
            if (rotten) rotten = 0;
            rotten = 1;
            while (reg--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_reg_attr;
                strcpy(foods->cells[y][x], food_reg);
            }   
            while (golden--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_golden_attr;
                strcpy(foods->cells[y][x], food_golden);
            } 
            while (magical--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_magical_attr;
                strcpy(foods->cells[y][x], food_magical);
            }
            while (rotten--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                foods->pickable_sit[y][x] = 1;
                foods->cnt[y][x] = 1;
                foods->attr[y][x] = food_rotten_attr;
                strcpy(foods->cells[y][x], food_rotten);
            }
        }
    }

    //weapon
    for (int i = 0; i < lv->room_num; i++) {
        
        if (i == lv->nightmare_room_id) {
            int sword = get_rand(0, 0);
            int dagger = get_rand(0, 0);
            int mace = get_rand(0, 0);
            int magical_wand = get_rand(0, 0);
            int arrow = get_rand(0, 0);
            while (sword--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 0;
                weapons->attr[y][x] = weapon_sword_attr;
                strcpy(weapons->cells[y][x], weapon_sword);
            }   
            while (dagger--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 0;
                weapons->attr[y][x] = weapon_dagger_attr;
                strcpy(weapons->cells[y][x], weapon_dagger);
            } 
            while (magical_wand--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 0;
                weapons->attr[y][x] = weapon_magic_wand_attr;
                strcpy(weapons->cells[y][x], weapon_magic_wand);
            }
            while (mace--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 0;
                weapons->attr[y][x] = weapon_mace_attr;
                strcpy(weapons->cells[y][x], weapon_mace);
            }
            while (arrow--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 0;
                weapons->attr[y][x] = weapon_arrow_attr;
                strcpy(weapons->cells[y][x], weapon_arrow);
            }   
        } else if(i == lv->treasure_room_id) {
            int sword = get_rand(0, 0);
            int dagger = get_rand(0, 0);
            int mace = get_rand(0, 0);
            int magical_wand = get_rand(0, 0);
            int arrow = get_rand(0, 1);
            while (sword--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 1;
                weapons->attr[y][x] = weapon_sword_attr;
                strcpy(weapons->cells[y][x], weapon_sword);
            }   
            while (dagger--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 10;
                weapons->attr[y][x] = weapon_dagger_attr;
                strcpy(weapons->cells[y][x], weapon_dagger);
            } 
            while (magical_wand--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 8;
                weapons->attr[y][x] = weapon_magic_wand_attr;
                strcpy(weapons->cells[y][x], weapon_magic_wand);
            }
            while (mace--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 0;
                weapons->attr[y][x] = weapon_mace_attr;
                strcpy(weapons->cells[y][x], weapon_mace);
            }
            while (arrow--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 20;
                weapons->attr[y][x] = weapon_arrow_attr;
                strcpy(weapons->cells[y][x], weapon_arrow);
            } 
        } else if (i == lv->enchant_room_id) {
            int sword = get_rand(0, 0);
            int dagger = get_rand(0, 0);
            int mace = get_rand(0, 0);
            int magical_wand = get_rand(1, 1);
            int arrow = get_rand(0, 0);
            while (sword--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 1;
                weapons->attr[y][x] = weapon_sword_attr;
                strcpy(weapons->cells[y][x], weapon_sword);
            }   
            while (dagger--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 10;
                weapons->attr[y][x] = weapon_dagger_attr;
                strcpy(weapons->cells[y][x], weapon_dagger);
            } 
            while (magical_wand--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 8;
                weapons->attr[y][x] = weapon_magic_wand_attr;
                strcpy(weapons->cells[y][x], weapon_magic_wand);
            }
            while (mace--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 0;
                weapons->attr[y][x] = weapon_mace_attr;
                strcpy(weapons->cells[y][x], weapon_mace);
            }
            while (arrow--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 20;
                weapons->attr[y][x] = weapon_arrow_attr;
                strcpy(weapons->cells[y][x], weapon_arrow);
            }  
        } else {
            int sword = get_rand(0, 3);
            if (sword) sword = 0;
            else sword = 1;
            int dagger = get_rand(0, 3);
            if (dagger) dagger = 0;
            else dagger = 1;
            int mace = get_rand(0, 0);
            int magical_wand = get_rand(0, 4);
            if (magical_wand) magical_wand = 0;
            else magical_wand = 1;
            int arrow = get_rand(0, 2);
            if (arrow) arrow = 0;
            else arrow = 1;
            while (sword--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 1;
                weapons->attr[y][x] = weapon_sword_attr;
                strcpy(weapons->cells[y][x], weapon_sword);
            }   
            while (dagger--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 10;
                weapons->attr[y][x] = weapon_dagger_attr;
                strcpy(weapons->cells[y][x], weapon_dagger);
            } 
            while (magical_wand--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 8;
                weapons->attr[y][x] = weapon_magic_wand_attr;
                strcpy(weapons->cells[y][x], weapon_magic_wand);
            }
            while (mace--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 0;
                weapons->attr[y][x] = weapon_mace_attr;
                strcpy(weapons->cells[y][x], weapon_mace);
            }
            while (arrow--) {
                elmnt *temp = give_free_elmnt_in_room(lv, i);
                int y = temp->y, x = temp->x;
                weapons->pickable_sit[y][x] = 1;
                weapons->cnt[y][x] = 20;
                weapons->attr[y][x] = weapon_arrow_attr;
                strcpy(weapons->cells[y][x], weapon_arrow);
            }   
        }
    }

}

map* generate_map(user *player) {
    map *mp = (map *)malloc(sizeof(map));
    mp->lvl_num = 4;
    mp->lvls = (lvl **)malloc(mp->lvl_num * sizeof(lvl*));
    for (int i = 0; i < mp->lvl_num; i++) {
        mp->lvls[i] = make_lvl(player->difficulty, i == 0, i == (mp->lvl_num - 1));
    }
    for (int i = 0; i < mp->lvl_num; i++) {
        // message_box(catnum("pick lvl ", i));
        add_pickable_things (player->difficulty, mp->lvls[i]);
    }
    for (int i = 0; i < mp->lvl_num; i++) {
        add_monsters(mp->lvls[i], player->difficulty);
    }
    mp->curr_lvl = 0;
    mp->hero_place.y = mp->lvls[0]->up_stair.y;
    mp->hero_place.x = mp->lvls[0]->up_stair.x;

    mp->inv = (Inventory *)malloc(sizeof(Inventory));
    
    mp->inv->gold_cnt = 0;

    mp->inv->food_golden_cnt = 0;
    mp->inv->food_magical_cnt = 0;
    mp->inv->food_reg_cnt = 0;
    mp->inv->food_rotten_cnt = 0;
    mp->inv->food_def = food_reg;
    mp->inv->food_def_attr = food_reg_attr;

    mp->inv->weapon_arrow_cnt = 0;
    mp->inv->weapon_dagger_cnt = 0;
    mp->inv->weapon_mace_cnt = 1;
    mp->inv->weapon_magic_wand_cnt = 0;
    mp->inv->weapon_sword_cnt = 0;
    mp->inv->weapon_def = weapon_mace;
    mp->inv->weapon_def_attr = weapon_mace_attr;

    mp->inv->talisman_damage_cnt = 0;
    mp->inv->talisman_health_cnt = 0;
    mp->inv->talisman_speed_cnt = 0; 
    mp->inv->talisman_def = talisman_health;
    mp->inv->talisman_def_attr = talisman_health_attr;

    mp->time = 0;

    for (int k = 0; k < mp->lvl_num; k++) {
        lvl *lv = mp->lvls[k];
        int src = lv->room_id[lv->up_stair.y][lv->up_stair.x];
        for (int i = lv->rooms[src]->starty; i < lv->rooms[src]->starty + lv->rooms[src]->h; i++) {
            for (int j = lv->rooms[src]->startx; j < lv->rooms[src]->startx + lv->rooms[src]->w; j++) {
                lv->explore_sit[i][j] = 1;
            }
        }
        for (int v = 0; v < lv->reg_room_num; v++) {
            if (v == src) continue;
            for (int i = lv->rooms[v]->starty; i < lv->rooms[v]->starty + lv->rooms[v]->h; i++) {
                lv->explore_sit[i][lv->rooms[v]->startx] = 1;
                lv->explore_sit[i][lv->rooms[v]->startx + lv->rooms[v]->w - 1] = 1;
            }
            for (int j = lv->rooms[v]->startx; j < lv->rooms[v]->startx + lv->rooms[v]->w; j++) {
                lv->explore_sit[lv->rooms[v]->starty][j] = 1;
                lv->explore_sit[lv->rooms[v]->starty + lv->rooms[v]->h - 1][j] = 1; 
            }
        }
        for (int i = 0; i < line_lvl; i++) {
            lv->explore_sit[i][0] = 1;
            lv->explore_sit[i][col_lvl - 1] = 1;
        }
        for (int j = 0; j < col_lvl; j++) {
            lv->explore_sit[0][j] = 1;
            lv->explore_sit[line_lvl - 1][j] = 1;
        }
    }
    
    mp->hp = 100;
    mp->stamina = 100;
    mp->enchant_damage = 0;
    mp->enchant_speed = 0;
    return mp;
}

void save_map (map *mp, user *player) {
    
    struct json_object *saver = json_object_new_object();
    struct json_object *mp_o = json_object_new_object();
    json_object_object_add(mp_o, "lvl_num", json_object_new_int64(mp->lvl_num));
    json_object_object_add(mp_o, "curr_lvl", json_object_new_int64(mp->curr_lvl));
    json_object_object_add(mp_o, "time", json_object_new_int64(mp->time));
    json_object_object_add(mp_o, "hp", json_object_new_int64(mp->hp));
    json_object_object_add(mp_o, "stamina", json_object_new_int64(mp->stamina));
    json_object_object_add(mp_o, "enchant_speed", json_object_new_int64(mp->enchant_speed));
    json_object_object_add(mp_o, "enchant_damage", json_object_new_int64(mp->enchant_damage));

    //hero place
    struct json_object *hero_place = json_object_new_object();
    json_object_object_add(hero_place, "y", json_object_new_int64(mp->hero_place.y));
    json_object_object_add(hero_place, "x", json_object_new_int64(mp->hero_place.x));
    json_object_object_add(mp_o, "hero_place", hero_place);

    // message_box("lvls:");

    //lvls
    for (int i = 0; i < mp->lvl_num; i++) {
        // message_box(catnum("level ", i));
        lvl *lv = mp->lvls[i];
        struct json_object *lv_o = json_object_new_object();
        struct json_object *cell = json_object_new_array();
        struct json_object *hidden_cell = json_object_new_array();
        struct json_object *hidden_sit = json_object_new_array();
        struct json_object *explore_sit = json_object_new_array();
        struct json_object *room_id = json_object_new_array();
        json_object_object_add(lv_o, "room_num", json_object_new_int64(lv->room_num));
        json_object_object_add(lv_o, "reg_room_num", json_object_new_int64(lv->reg_room_num));
        json_object_object_add(lv_o, "nightmare_room_id", json_object_new_int64(lv->nightmare_room_id));
        json_object_object_add(lv_o, "treasure_room_id", json_object_new_int64(lv->treasure_room_id));
        json_object_object_add(lv_o, "enchant_room_id", json_object_new_int64(lv->enchant_room_id));
        json_object_object_add(lv_o, "monster_num", json_object_new_int64(lv->monster_num));


        for (int y = 0; y < line_lvl; y++) {
            for (int x = 0; x < col_lvl; x++) {
                json_object_array_add(cell, json_object_new_int64(lv->cell[y][x]));
                json_object_array_add(hidden_cell, json_object_new_int64(lv->hidden_cell[y][x]));
                json_object_array_add(hidden_sit, json_object_new_int64(lv->hidden_sit[y][x]));
                json_object_array_add(explore_sit, json_object_new_int64(lv->explore_sit[y][x]));
                json_object_array_add(room_id, json_object_new_int64(lv->room_id[y][x]));
            }
        } 
        json_object_object_add(lv_o, "cell", cell);
        json_object_object_add(lv_o, "hidden_cell", hidden_cell);
        json_object_object_add(lv_o, "hidden_sit", hidden_sit);
        json_object_object_add(lv_o, "explore_sit", explore_sit);
        json_object_object_add(lv_o, "room_id", room_id);

        // message_box("rooms:");
        //rooms
        for (int j = 0; j < lv->room_num; j++) {
            room* rm = lv->rooms[j];
            struct json_object *rm_o = json_object_new_object();
            json_object_object_add(rm_o, "starty", json_object_new_int64(rm->starty));
            json_object_object_add(rm_o, "startx", json_object_new_int64(rm->startx));
            json_object_object_add(rm_o, "h", json_object_new_int64(rm->h));
            json_object_object_add(rm_o, "w", json_object_new_int64(rm->w));
            json_object_object_add(rm_o, "door_num", json_object_new_int64(rm->door_num));

            for (int k = 0; k < lv->rooms[j]->door_num; k++) {
                struct json_object *dr_o = json_object_new_object();
                json_object_object_add(dr_o, "y", json_object_new_int64(rm->door[j].y));
                json_object_object_add(dr_o, "x", json_object_new_int64(rm->door[j].x));
                json_object_object_add(rm_o, catnum("door", k), dr_o);
            }

            json_object_object_add(lv_o, catnum("rooms", j), rm_o);
        }
        //monster
        for (int j = 0; j < lv->monster_num; j++) {
            enemy *mon = lv->monster[j];
            struct json_object *mon_o = json_object_new_object();
            json_object_object_add(mon_o, "look", json_object_new_int64(mon->look));
            json_object_object_add(mon_o, "attr", json_object_new_int64(mon->attr));
            json_object_object_add(mon_o, "max_hp", json_object_new_int64(mon->max_hp));
            json_object_object_add(mon_o, "max_token", json_object_new_int64(mon->max_token));
            json_object_object_add(mon_o, "damage", json_object_new_int64(mon->damage));
            json_object_object_add(mon_o, "name", json_object_new_string(mon->name));
            struct json_object *cnt = json_object_new_array();
            struct json_object *hp = json_object_new_array();
            struct json_object *token = json_object_new_array();
            for (int y = 0; y < line_lvl; y++) {
                for (int x = 0; x < col_lvl; x++) {
                    json_object_array_add(cnt, json_object_new_int64(mon->cnt[y][x]));
                    json_object_array_add(hp, json_object_new_int64(mon->hp[y][x]));
                    json_object_array_add(token, json_object_new_int64(mon->token[y][x]));
                }
            }
            json_object_object_add(mon_o, "cnt", cnt); 
            json_object_object_add(mon_o, "hp", hp); 
            json_object_object_add(mon_o, "token", token);
            json_object_object_add(lv_o, catnum("monster", j), mon_o);       

        }
        
        //up_stair
        struct json_object *up_stair = json_object_new_object();
        json_object_object_add(up_stair, "y", json_object_new_int64(lv->up_stair.y));
        json_object_object_add(up_stair, "x", json_object_new_int64(lv->up_stair.x));
        json_object_object_add(lv_o, "up_stair", up_stair);
        //down_stair
        struct json_object *down_stair = json_object_new_object();
        json_object_object_add(down_stair, "y", json_object_new_int64(lv->down_stair.y));
        json_object_object_add(down_stair, "x", json_object_new_int64(lv->down_stair.x));
        json_object_object_add(lv_o, "down_stair", down_stair);

        // message_box("pk things:");
        //pickable_things
        pickable_things *pk_thing;
        struct json_object *pk_thing_o;
        struct json_object *pickable_sit;
        struct json_object *cells;
        struct json_object *attr;
        struct json_object *cnt;
        //golds
        pk_thing = lv->golds;
        pk_thing_o = json_object_new_object();
        pickable_sit = json_object_new_array();
        cells = json_object_new_array();
        attr= json_object_new_array();
        cnt = json_object_new_array();
        for (int y = 0; y < line_lvl; y++) {
            for (int x = 0; x < col_lvl; x++) {
                json_object_array_add(pickable_sit, json_object_new_int64(pk_thing->pickable_sit[y][x]));
                json_object_array_add(cells, json_object_new_string(pk_thing->cells[y][x]));
                json_object_array_add(attr, json_object_new_int64(pk_thing->attr[y][x]));
                json_object_array_add(cnt, json_object_new_int64(pk_thing->cnt[y][x]));
            }
        }
        json_object_object_add(pk_thing_o, "pickable_sit", pickable_sit);
        json_object_object_add(pk_thing_o, "cells", cells);
        json_object_object_add(pk_thing_o, "attr", attr);
        json_object_object_add(pk_thing_o, "cnt", cnt);

        json_object_object_add(lv_o, "golds", pk_thing_o);
        //weapons
        pk_thing = lv->weapons;
        pk_thing_o = json_object_new_object();
        pickable_sit = json_object_new_array();
        cells = json_object_new_array();
        attr= json_object_new_array();
        cnt = json_object_new_array();
        for (int y = 0; y < line_lvl; y++) {
            for (int x = 0; x < col_lvl; x++) {
                json_object_array_add(pickable_sit, json_object_new_int64(pk_thing->pickable_sit[y][x]));
                json_object_array_add(cells, json_object_new_string(pk_thing->cells[y][x]));
                json_object_array_add(attr, json_object_new_int64(pk_thing->attr[y][x]));
                json_object_array_add(cnt, json_object_new_int64(pk_thing->cnt[y][x]));
            }
        }
        json_object_object_add(pk_thing_o, "pickable_sit", pickable_sit);
        json_object_object_add(pk_thing_o, "cells", cells);
        json_object_object_add(pk_thing_o, "attr", attr);
        json_object_object_add(pk_thing_o, "cnt", cnt);

        json_object_object_add(lv_o, "weapons", pk_thing_o);
        //talismans
        pk_thing = lv->talismans;
        pk_thing_o = json_object_new_object();
        pickable_sit = json_object_new_array();
        cells = json_object_new_array();
        attr= json_object_new_array();
        cnt = json_object_new_array();
        for (int y = 0; y < line_lvl; y++) {
            for (int x = 0; x < col_lvl; x++) {
                json_object_array_add(pickable_sit, json_object_new_int64(pk_thing->pickable_sit[y][x]));
                json_object_array_add(cells, json_object_new_string(pk_thing->cells[y][x]));
                json_object_array_add(attr, json_object_new_int64(pk_thing->attr[y][x]));
                json_object_array_add(cnt, json_object_new_int64(pk_thing->cnt[y][x]));
            }
        }
        json_object_object_add(pk_thing_o, "pickable_sit", pickable_sit);
        json_object_object_add(pk_thing_o, "cells", cells);
        json_object_object_add(pk_thing_o, "attr", attr);
        json_object_object_add(pk_thing_o, "cnt", cnt);

        json_object_object_add(lv_o, "talismans", pk_thing_o);
        //foods
        pk_thing = lv->foods;
        pk_thing_o = json_object_new_object();
        pickable_sit = json_object_new_array();
        cells = json_object_new_array();
        attr= json_object_new_array();
        cnt = json_object_new_array();
        for (int y = 0; y < line_lvl; y++) {
            for (int x = 0; x < col_lvl; x++) {
                json_object_array_add(pickable_sit, json_object_new_int64(pk_thing->pickable_sit[y][x]));
                json_object_array_add(cells, json_object_new_string(pk_thing->cells[y][x]));
                json_object_array_add(attr, json_object_new_int64(pk_thing->attr[y][x]));
                json_object_array_add(cnt, json_object_new_int64(pk_thing->cnt[y][x]));
            }
        }
        json_object_object_add(pk_thing_o, "pickable_sit", pickable_sit);
        json_object_object_add(pk_thing_o, "cells", cells);
        json_object_object_add(pk_thing_o, "attr", attr);
        json_object_object_add(pk_thing_o, "cnt", cnt);

        json_object_object_add(lv_o, "foods", pk_thing_o);
        //
        json_object_object_add(mp_o, catnum("lvls", i), lv_o);
    }

    // message_box("inv :");
    //inv
    struct json_object *inv_o = json_object_new_object();
    json_object_object_add(inv_o, "gold_cnt", json_object_new_int64(mp->inv->gold_cnt));
    json_object_object_add(inv_o, "food_golden_cnt", json_object_new_int64(mp->inv->food_golden_cnt));
    json_object_object_add(inv_o, "food_magical_cnt", json_object_new_int64(mp->inv->food_magical_cnt));
    json_object_object_add(inv_o, "food_reg_cnt", json_object_new_int64(mp->inv->food_reg_cnt));
    json_object_object_add(inv_o, "food_rotten_cnt", json_object_new_int64(mp->inv->food_rotten_cnt));
    json_object_object_add(inv_o, "food_def", json_object_new_string(mp->inv->food_def));//
    json_object_object_add(inv_o, "food_def_attr", json_object_new_int64(mp->inv->food_def_attr));
    json_object_object_add(inv_o, "weapon_arrow_cnt", json_object_new_int64(mp->inv->weapon_arrow_cnt));
    json_object_object_add(inv_o, "weapon_dagger_cnt", json_object_new_int64(mp->inv->weapon_dagger_cnt));
    json_object_object_add(inv_o, "weapon_mace_cnt", json_object_new_int64(mp->inv->weapon_mace_cnt));
    json_object_object_add(inv_o, "weapon_magic_wand_cnt", json_object_new_int64(mp->inv->weapon_magic_wand_cnt));
    json_object_object_add(inv_o, "weapon_sword_cnt", json_object_new_int64(mp->inv->weapon_sword_cnt));
    json_object_object_add(inv_o, "weapon_def", json_object_new_string(mp->inv->weapon_def));//
    json_object_object_add(inv_o, "weapon_def_attr", json_object_new_int64(mp->inv->weapon_def_attr));
    json_object_object_add(inv_o, "talisman_damage_cnt", json_object_new_int64(mp->inv->talisman_damage_cnt));
    json_object_object_add(inv_o, "talisman_health_cnt", json_object_new_int64(mp->inv->talisman_health_cnt));
    json_object_object_add(inv_o, "talisman_speed_cnt", json_object_new_int64(mp->inv->talisman_speed_cnt));
    json_object_object_add(inv_o, "talisman_def", json_object_new_string(mp->inv->talisman_def));//
    json_object_object_add(inv_o, "talisman_def_attr", json_object_new_int64(mp->inv->talisman_def_attr));

    json_object_object_add(mp_o, "inv", inv_o);
    

    json_object_object_add(saver, "map", mp_o);

    FILE *data = fopen(player->last_save_of_game, "w");
    // if (data) {
    //     const char *json_str = json_object_to_json_string_ext(saver, JSON_C_TO_STRING_PRETTY);
    //     size_t len = strlen(json_str);
    //     if (fwrite(json_str, 1, len, data) != len) {
    //         message_box("Couldn't save your game!");
    //     }
    //     fclose(data);
    // } else {
    //     message_box("Couldn't save your game, sorry!");
    // }

    if (data) {
        fprintf(data, "%s", json_object_to_json_string(saver));
        fclose(data);
    } else {
        message_box("Couldn't save your game, sorry!");
    }
    
    // json_object_put(saver);
}

map* load_map (user *player) {
    FILE *data = fopen(player->last_save_of_game, "r");
    if (!data) {
        message_box("There is no game for continue. Creat new game first");
        return NULL;
    }
    fseek(data, 0, SEEK_END);
    size_t fsz = ftell(data);
    fseek(data, 0, SEEK_SET);
    char *buffer = (char *)malloc((fsz + 10) * sizeof(char));
    int fsz2 = fread(buffer, 1, fsz + 10, data);
    fclose(data);
    buffer[fsz2] = '\0'; //fsz == fsz2;

    struct json_object *src = json_tokener_parse(buffer);
    if (!src) {
        message_box("There is no game for continue. Creat new game first");
        return NULL;
    }

    map *mp = (map *)malloc(sizeof(map));
    struct json_object *mp_o = json_object_object_get(src, "map");
    
    mp->lvl_num = json_object_get_int64(json_object_object_get(mp_o, "lvl_num"));
    mp->curr_lvl = json_object_get_int64(json_object_object_get(mp_o, "curr_lvl"));
    mp->time = json_object_get_int64(json_object_object_get(mp_o, "time"));
    mp->hp = json_object_get_int64(json_object_object_get(mp_o, "hp"));
    mp->stamina = json_object_get_int64(json_object_object_get(mp_o, "stamina"));
    mp->enchant_speed = json_object_get_int64(json_object_object_get(mp_o, "enchant_speed"));
    mp->enchant_damage = json_object_get_int64(json_object_object_get(mp_o, "enchant_damage"));
    
    mp->hero_place.y = json_object_get_int64(json_object_object_get(json_object_object_get(mp_o, "hero_place"), "y"));
    mp->hero_place.x = json_object_get_int64(json_object_object_get(json_object_object_get(mp_o, "hero_place"), "x"));

    // message_box("lvls:");
    //lvls
    mp->lvls = (lvl **)malloc(mp->lvl_num * sizeof(lvl *));
    for (int i = 0; i < mp->lvl_num; i++) {
        // message_box(catnum("level :", i));
        lvl *lv = (lvl *)malloc(sizeof(lvl));
        struct json_object *lv_o = json_object_object_get(mp_o, catnum("lvls", i));
        lv->room_num = json_object_get_int64(json_object_object_get(lv_o, "room_num"));
        lv->reg_room_num = json_object_get_int64(json_object_object_get(lv_o, "reg_room_num"));
        lv->nightmare_room_id = json_object_get_int64(json_object_object_get(lv_o, "nightmare_room_id"));
        lv->treasure_room_id = json_object_get_int64(json_object_object_get(lv_o, "treasure_room_id"));
        lv->enchant_room_id = json_object_get_int64(json_object_object_get(lv_o, "enchant_room_id"));
        lv->monster_num = json_object_get_int64(json_object_object_get(lv_o, "monster_num"));

        lv->up_stair.y = json_object_get_int64(json_object_object_get(json_object_object_get(lv_o, "up_stair"), "y"));
        lv->up_stair.x = json_object_get_int64(json_object_object_get(json_object_object_get(mp_o, "up_stair"), "x"));

        lv->down_stair.y = json_object_get_int64(json_object_object_get(json_object_object_get(lv_o, "down_stair"), "y"));
        lv->down_stair.x = json_object_get_int64(json_object_object_get(json_object_object_get(mp_o, "down_stair"), "x"));

        lv->cell = (chtype **)malloc(line_lvl * sizeof(chtype *));
        struct json_object *cell = json_object_object_get(lv_o, "cell");
        for (int y = 0; y < line_lvl; y++) {
            lv->cell[y] = (chtype *)malloc(col_lvl * sizeof(chtype));
            for (int x = 0; x < col_lvl; x++) {
                lv->cell[y][x] = json_object_get_int64(json_object_array_get_idx(cell, y * col_lvl + x));
            }
        }

        lv->hidden_cell = (chtype **)malloc(line_lvl * sizeof(chtype *));
        struct json_object *hidden_cell = json_object_object_get(lv_o, "hidden_cell");
        for (int y = 0; y < line_lvl; y++) {
            lv->hidden_cell[y] = (chtype *)malloc(col_lvl * sizeof(chtype));
            for (int x = 0; x < col_lvl; x++) {
                lv->hidden_cell[y][x] = json_object_get_int64(json_object_array_get_idx(hidden_cell, y * col_lvl + x));
            }
        }

        lv->hidden_sit = (int **)malloc(line_lvl * sizeof(int *));
        struct json_object *hidden_sit = json_object_object_get(lv_o, "hidden_sit");
        for (int y = 0; y < line_lvl; y++) {
            lv->hidden_sit[y] = (int *)malloc(col_lvl * sizeof(int));
            for (int x = 0; x < col_lvl; x++) {
                lv->hidden_sit[y][x] = json_object_get_int64(json_object_array_get_idx(hidden_sit, y * col_lvl + x));
            }
        }

        lv->room_id = (int **)malloc(line_lvl * sizeof(int *));
        struct json_object *room_id = json_object_object_get(lv_o, "room_id");
        for (int y = 0; y < line_lvl; y++) {
            lv->room_id[y] = (int *)malloc(col_lvl * sizeof(int));
            for (int x = 0; x < col_lvl; x++) {
                lv->room_id[y][x] = json_object_get_int64(json_object_array_get_idx(room_id, y * col_lvl + x));
            }
        }
        
        lv->explore_sit = (int **)malloc(line_lvl * sizeof(int *));
        struct json_object *explore_sit = json_object_object_get(lv_o, "explore_sit");
        for (int y = 0; y < line_lvl; y++) {
            lv->explore_sit[y] = (int *)malloc(col_lvl * sizeof(int));
            for (int x = 0; x < col_lvl; x++) {
                lv->explore_sit[y][x] = json_object_get_int64(json_object_array_get_idx(explore_sit, y * col_lvl + x));
            }
        }

        // message_box("rooms");
        //rooms
        lv->rooms = (room **)malloc(lv->room_num * sizeof(room *));
        for (int j = 0; j < lv->room_num; j++) {
            struct json_object *rm_o = json_object_object_get(lv_o, catnum("rooms", j));
            room *rm = (room *)malloc(sizeof(room));
            rm->starty = json_object_get_int64(json_object_object_get(rm_o, "starty"));
            rm->startx = json_object_get_int64(json_object_object_get(rm_o, "startx")); 
            rm->h = json_object_get_int64(json_object_object_get(rm_o, "h"));
            rm->w = json_object_get_int64(json_object_object_get(rm_o, "w"));
            rm->door_num = json_object_get_int64(json_object_object_get(rm_o, "door_num"));
            rm->door = (elmnt *)malloc(rm->door_num * sizeof(elmnt));
            for (int k = 0; k < rm->door_num; k++) {
                struct json_object *dr_o = json_object_object_get(rm_o, catnum("door", k));
                rm->door[k].y = json_object_get_int64(json_object_object_get(dr_o, "y"));
                rm->door[k].x = json_object_get_int64(json_object_object_get(dr_o, "x")); 
            }

            lv->rooms[j] = rm;
        }

        // message_box("monsters:");
        //monster
        lv->monster = (enemy **)malloc(lv->monster_num * sizeof(enemy *));
        for (int j = 0; j < lv->monster_num; j++) {
            // message_box(catnum("monster: ", j));
            struct json_object *mon_o = json_object_object_get(lv_o, catnum("monster", j));
            enemy *mon = (enemy *)malloc(sizeof(enemy));

            mon->look = json_object_get_int64(json_object_object_get(mon_o, "look"));
            mon->name = (char *)malloc(50 * sizeof(char)); mon->name[0] = '\0';
            strcpy(mon->name, json_object_get_string(json_object_object_get(mon_o, "name")));//
            mon->attr = json_object_get_int64(json_object_object_get(mon_o, "attr"));
            mon->max_hp = json_object_get_int64(json_object_object_get(mon_o, "max_hp"));
            mon->max_token = json_object_get_int64(json_object_object_get(mon_o, "max_token"));
            mon->damage = json_object_get_int64(json_object_object_get(mon_o, "damage"));

            // message_box("cnt :");
            mon->cnt = (int **)malloc(line_lvl * sizeof(int *));
            struct json_object *cnt = json_object_object_get(mon_o, "cnt");
            for (int y = 0; y < line_lvl; y++) {
                mon->cnt[y] = (int *)malloc(col_lvl * sizeof(int));
                for (int x = 0; x < col_lvl; x++) {
                    mon->cnt[y][x] = json_object_get_int64(json_object_array_get_idx(cnt, y * col_lvl + x));
                }
            }
            // message_box("token :");
            mon->token = (int **)malloc(line_lvl * sizeof(int *));
            struct json_object *token = json_object_object_get(mon_o, "token");
            for (int y = 0; y < line_lvl; y++) {
                mon->token[y] = (int *)malloc(col_lvl * sizeof(int));
                for (int x = 0; x < col_lvl; x++) {
                    mon->token[y][x] = json_object_get_int64(json_object_array_get_idx(token, y * col_lvl + x));
                }
            }

            mon->hp = (int **)malloc(line_lvl * sizeof(int *));
            struct json_object *hp = json_object_object_get(mon_o, "hp");
            for (int y = 0; y < line_lvl; y++) {
                mon->hp[y] = (int *)malloc(col_lvl * sizeof(int));
                for (int x = 0; x < col_lvl; x++) {
                    mon->hp[y][x] = json_object_get_int64(json_object_array_get_idx(hp, y * col_lvl + x));
                }
            }

            lv->monster[j] = mon;
        }

        // message_box("pk things:");
        //pickable things
        pickable_things *pk_thing;
        struct json_object *pk_thing_o;
        struct json_object *pickable_sit;
        struct json_object *cells;
        struct json_object *attr;
        struct json_object *cnt;
        //golds
        pk_thing = (pickable_things *)malloc(sizeof(pickable_things));
        lv->golds = pk_thing;
        pk_thing->pickable_sit = (int **)malloc(line_lvl * sizeof(int));
        pk_thing->cells = (char ***)malloc(line_lvl * sizeof(char **));
        pk_thing->attr = (chtype **)malloc(line_lvl * sizeof(chtype *));
        pk_thing->cnt = (int **)malloc(line_lvl * sizeof(int *));

        pk_thing_o = json_object_object_get(lv_o, "golds");
        pickable_sit = json_object_object_get(pk_thing_o, "pickable_sit");
        cells = json_object_object_get(pk_thing_o, "cells");
        attr= json_object_object_get(pk_thing_o, "attr");
        cnt = json_object_object_get(pk_thing_o, "cnt");

        for (int y = 0; y < line_lvl; y++) {
            pk_thing->pickable_sit[y] = (int *)malloc(col_lvl * sizeof(int));
            pk_thing->cells[y] = (char **)malloc(col_lvl * sizeof(char *));
            pk_thing->attr[y] = (chtype *)malloc(col_lvl * sizeof(chtype));
            pk_thing->cnt[y] = (int *)malloc(col_lvl * sizeof(int));

            for (int x = 0; x < col_lvl; x++) {
                pk_thing->pickable_sit[y][x] = json_object_get_int64(json_object_array_get_idx(pickable_sit, y * col_lvl + x));
                pk_thing->attr[y][x] = json_object_get_int64(json_object_array_get_idx(attr, y * col_lvl + x));
                pk_thing->cnt[y][x] = json_object_get_int64(json_object_array_get_idx(cnt, y * col_lvl + x));
                pk_thing->cells[y][x] = (char *)malloc(50 * sizeof(char)); pk_thing->cells[y][x][0] = '\0';
                // strcpy(pk_thing->cells[y][x], json_object_get_string(json_object_array_get_idx(cells, y * col_lvl + x)));
            }
        }

        //weapons
        pk_thing = (pickable_things *)malloc(sizeof(pickable_things));
        lv->weapons = pk_thing;
        pk_thing->pickable_sit = (int **)malloc(line_lvl * sizeof(int));
        pk_thing->cells = (char ***)malloc(line_lvl * sizeof(char **));
        pk_thing->attr = (chtype **)malloc(line_lvl * sizeof(chtype *));
        pk_thing->cnt = (int **)malloc(line_lvl * sizeof(int *));

        pk_thing_o = json_object_object_get(lv_o, "weapons");
        pickable_sit = json_object_object_get(pk_thing_o, "pickable_sit");
        cells = json_object_object_get(pk_thing_o, "cells");
        attr= json_object_object_get(pk_thing_o, "attr");
        cnt = json_object_object_get(pk_thing_o, "cnt");

        for (int y = 0; y < line_lvl; y++) {
            pk_thing->pickable_sit[y] = (int *)malloc(col_lvl * sizeof(int));
            pk_thing->cells[y] = (char **)malloc(col_lvl * sizeof(char *));
            pk_thing->attr[y] = (chtype *)malloc(col_lvl * sizeof(chtype));
            pk_thing->cnt[y] = (int *)malloc(col_lvl * sizeof(int));

            for (int x = 0; x < col_lvl; x++) {
                pk_thing->pickable_sit[y][x] = json_object_get_int64(json_object_array_get_idx(pickable_sit, y * col_lvl + x));
                pk_thing->attr[y][x] = json_object_get_int64(json_object_array_get_idx(attr, y * col_lvl + x));
                pk_thing->cnt[y][x] = json_object_get_int64(json_object_array_get_idx(cnt, y * col_lvl + x));
                pk_thing->cells[y][x] = (char *)malloc(50 * sizeof(char)); pk_thing->cells[y][x][0] = '\0';
                // strcpy(pk_thing->cells[y][x], json_object_get_string(json_object_array_get_idx(cells, y * col_lvl + x)));
            }
        }

        //talismans
        pk_thing = (pickable_things *)malloc(sizeof(pickable_things));
        lv->talismans = pk_thing;
        pk_thing->pickable_sit = (int **)malloc(line_lvl * sizeof(int));
        pk_thing->cells = (char ***)malloc(line_lvl * sizeof(char **));
        pk_thing->attr = (chtype **)malloc(line_lvl * sizeof(chtype *));
        pk_thing->cnt = (int **)malloc(line_lvl * sizeof(int *));

        pk_thing_o = json_object_object_get(lv_o, "talismans");
        pickable_sit = json_object_object_get(pk_thing_o, "pickable_sit");
        cells = json_object_object_get(pk_thing_o, "cells");
        attr= json_object_object_get(pk_thing_o, "attr");
        cnt = json_object_object_get(pk_thing_o, "cnt");

        for (int y = 0; y < line_lvl; y++) {
            pk_thing->pickable_sit[y] = (int *)malloc(col_lvl * sizeof(int));
            pk_thing->cells[y] = (char **)malloc(col_lvl * sizeof(char *));
            pk_thing->attr[y] = (chtype *)malloc(col_lvl * sizeof(chtype));
            pk_thing->cnt[y] = (int *)malloc(col_lvl * sizeof(int));

            for (int x = 0; x < col_lvl; x++) {
                pk_thing->pickable_sit[y][x] = json_object_get_int64(json_object_array_get_idx(pickable_sit, y * col_lvl + x));
                pk_thing->attr[y][x] = json_object_get_int64(json_object_array_get_idx(attr, y * col_lvl + x));
                pk_thing->cnt[y][x] = json_object_get_int64(json_object_array_get_idx(cnt, y * col_lvl + x));
                pk_thing->cells[y][x] = (char *)malloc(50 * sizeof(char)); pk_thing->cells[y][x][0] = '\0';
                // strcpy(pk_thing->cells[y][x], json_object_get_string(json_object_array_get_idx(cells, y * col_lvl + x)));
            }
        }

        //foods
        pickable_things *pk_2;
        pk_2 = (pickable_things *)malloc(sizeof(pickable_things));
        lv->foods = pk_2;
        pk_2->pickable_sit = (int **)malloc(line_lvl * sizeof(int));
        pk_2->cells = (char ***)malloc(line_lvl * sizeof(char **));
        pk_2->attr = (chtype **)malloc(line_lvl * sizeof(chtype *));
        pk_2->cnt = (int **)malloc(line_lvl * sizeof(int *));

        pk_thing_o = json_object_object_get(lv_o, "foods");
        pickable_sit = json_object_object_get(pk_thing_o, "pickable_sit");
        cells = json_object_object_get(pk_thing_o, "cells");
        attr= json_object_object_get(pk_thing_o, "attr");
        cnt = json_object_object_get(pk_thing_o, "cnt");

        for (int y = 0; y < line_lvl; y++) {
            pk_2->pickable_sit[y] = (int *)malloc(col_lvl * sizeof(int));
            pk_2->cells[y] = (char **)malloc(col_lvl * sizeof(char *));
            pk_2->attr[y] = (chtype *)malloc(col_lvl * sizeof(chtype));
            pk_2->cnt[y] = (int *)malloc(col_lvl * sizeof(int));

            for (int x = 0; x < col_lvl; x++) {
                pk_2->pickable_sit[y][x] = json_object_get_int64(json_object_array_get_idx(pickable_sit, y * col_lvl + x));
                pk_2->attr[y][x] = json_object_get_int64(json_object_array_get_idx(attr, y * col_lvl + x));
                pk_2->cnt[y][x] = json_object_get_int64(json_object_array_get_idx(cnt, y * col_lvl + x));
                // pk_2->cells[y][x] = (char *)malloc(50 * sizeof(char)); pk_2->cells[y][x][1] = '\0'; pk_2->cells[y][x][0] = 'a';
                pk_2->cells[y][x] = "aa";
                if (pk_2->cells[y][x] == NULL) message_box("fuckuuu !");

                // strcpy(pk_2->cells[y][x], json_object_get_string(json_object_array_get_idx(cells, y * col_lvl + x)));
            }
        }
        for (int y = 0; y < line_lvl; y++) {
            for (int x = 0; x < col_lvl; x++) {
                if (pk_2->cells[y][x] == NULL) {
                    if (pk_2->cells[y][x] == NULL) message_box("fuck!");
                }
                    
            }
        }

        mp->lvls[i] = lv;
    }

    // message_box("inv:");
    //inv
    mp->inv = (Inventory *)malloc(sizeof(Inventory));
    struct json_object *inv_o = json_object_object_get(mp_o, "inv");
    mp->inv->gold_cnt = json_object_get_int64(json_object_object_get(inv_o, "gold_cnt"));
    mp->inv->food_golden_cnt = json_object_get_int64(json_object_object_get(inv_o, "food_golden_cnt"));
    mp->inv->food_magical_cnt = json_object_get_int64(json_object_object_get(inv_o, "food_magical_cnt"));
    mp->inv->food_reg_cnt = json_object_get_int64(json_object_object_get(inv_o, "food_reg_cnt"));
    mp->inv->food_rotten_cnt = json_object_get_int64(json_object_object_get(inv_o, "food_rotten_cnt"));

    mp->inv->food_def = (char *)malloc(50 * sizeof(char)); mp->inv->food_def[0] = '\0';
    strcpy(mp->inv->food_def, json_object_get_string(json_object_object_get(inv_o, "food_def")));//

    mp->inv->food_def_attr = json_object_get_int64(json_object_object_get(inv_o, "food_def_attr"));
    mp->inv->weapon_arrow_cnt = json_object_get_int64(json_object_object_get(inv_o, "weapon_arrow_cnt"));
    mp->inv->weapon_dagger_cnt = json_object_get_int64(json_object_object_get(inv_o, "weapon_dagger_cnt"));
    mp->inv->weapon_mace_cnt = json_object_get_int64(json_object_object_get(inv_o, "weapon_mace_cnt"));
    mp->inv->weapon_magic_wand_cnt = json_object_get_int64(json_object_object_get(inv_o, "weapon_magic_wand_cnt"));
    mp->inv->weapon_sword_cnt = json_object_get_int64(json_object_object_get(inv_o, "weapon_sword_cnt"));

    mp->inv->weapon_def = (char *)malloc(50 * sizeof(char)); mp->inv->weapon_def[0] = '\0';
    strcpy(mp->inv->weapon_def, json_object_get_string(json_object_object_get(inv_o, "weapon_def")));

    mp->inv->weapon_def_attr = json_object_get_int64(json_object_object_get(inv_o, "weapon_def_attr"));
    mp->inv->talisman_damage_cnt = json_object_get_int64(json_object_object_get(inv_o, "talisman_damage_cnt"));
    mp->inv->talisman_health_cnt = json_object_get_int64(json_object_object_get(inv_o, "talisman_health_cnt"));
    mp->inv->talisman_speed_cnt = json_object_get_int64(json_object_object_get(inv_o, "talisman_speed_cnt"));

    mp->inv->talisman_def = (char *)malloc(50 * sizeof(char)); mp->inv->talisman_def[0] = '\0';
    strcpy(mp->inv->talisman_def, json_object_get_string(json_object_object_get(inv_o, "talisman_def")));
    mp->inv->talisman_def_attr = json_object_get_int64(json_object_object_get(inv_o, "talisman_def_attr"));

    // json_object_put(src);
    return mp;

}

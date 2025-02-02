#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>

#include "menu.h"
#include "map.h"

int hp, Max_hp, hp_bar_len;
int stamina, Max_stamina, stamina_bar_len;
int enchant_speed, enchant_damage, Max_enchant, enchant_bar_len;
int M_mode_on;
WINDOW *bar_win;
WINDOW *mp_win;
WINDOW *info_win;
WINDOW *mg_win;


void init_windows() {
    //left side win
    WINDOW *win = newwin(LINES, delta_x, 0, 0);
    int x = YELLOW_ON_BLACK;
    wattron(win, COLOR_PAIR(BLUE_ON_BLACK));
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER | COLOR_PAIR(x),
                    ACS_URCORNER | COLOR_PAIR(x), ACS_LLCORNER | COLOR_PAIR(x),
                    ACS_LRCORNER | COLOR_PAIR(x));
    for (int i = 1; i < delta_x - 1; i++) {
        mvwaddch(win, delta_y, i, ACS_HLINE);
    }
    wrefresh(win);

    //message win
    win = newwin(delta_y, col_lvl, line_lvl, startx_lvl);
    wattron(win, COLOR_PAIR(BLUE_ON_BLACK));
    wborder(win, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER | COLOR_PAIR(x),
                    ACS_URCORNER | COLOR_PAIR(x), ACS_LLCORNER | COLOR_PAIR(x),
                    ACS_LRCORNER | COLOR_PAIR(x));
    wattron(win, A_BOLD |COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(win, 0, 5, "Message");
    wrefresh(win);
}

void init_elements_movement() {
    Max_hp = 100;
    Max_stamina = 100;
    Max_enchant = 100;
    hp_bar_len = 30;
    stamina_bar_len = 20;
    enchant_bar_len = 10;
    M_mode_on = 0;

    bar_win = NULL;
    mp_win = NULL;
    info_win = NULL;
    mg_win = NULL;
    init_windows();
}

void show_message_cover (char *s, int line_num) {
    int startx = startx_lvl + 1, starty = line_lvl + 1, len_y = delta_y - 2, len_x = col_lvl - 2;
    if (mg_win == NULL) mg_win = newwin(len_y, len_x, starty, startx);
    else wclear(mg_win);
    int c = 222;
    init_pair(c, c, BLACK);
    wattron(mg_win, COLOR_PAIR(c));
    mvwprintw(mg_win, line_num, 1, "%s", s);
    wattroff(mg_win, COLOR_PAIR(c));
    wrefresh(mg_win);
}

void show_message_continue (char *s) {
    int startx = startx_lvl + 1, starty = line_lvl + 1, len_y = delta_y - 2, len_x = col_lvl - 2;
    if (mg_win == NULL) mg_win = newwin(len_y, len_x, starty, startx);
    int c = 222;
    init_pair(c, c, BLACK);
    wattron(mg_win, COLOR_PAIR(c));
    wprintw(mg_win, "%s", s);
    wattroff(mg_win, COLOR_PAIR(c));
    wrefresh(mg_win);
}

void show_message_cover_int (int n, int line_num) {
    int startx = startx_lvl + 1, starty = line_lvl + 1, len_y = delta_y - 2, len_x = col_lvl - 2;
    if (mg_win == NULL) mg_win = newwin(len_y, len_x, starty, startx);
    else wclear(mg_win);
    int c = 222;
    init_pair(c, c, BLACK);
    wattron(mg_win, COLOR_PAIR(c));
    mvwprintw(mg_win, line_num, 1, "%d", n);
    wattroff(mg_win, COLOR_PAIR(c));
    wrefresh(mg_win);
}

void show_message_continue_int (int n) {
    int startx = startx_lvl + 1, starty = line_lvl + 1, len_y = delta_y - 2, len_x = col_lvl - 2;
    if (mg_win == NULL) mg_win = newwin(len_y, len_x, starty, startx);
    int c = 222;
    init_pair(c, c, BLACK);
    wattron(mg_win, COLOR_PAIR(c));
    wprintw(mg_win, "%d", n);
    wattroff(mg_win, COLOR_PAIR(c));
    wrefresh(mg_win);
}

void power_bar_show () { //1:dp, 2:stamina, 3:speed, 4:damage;
    int startx = 1, starty = 1, len_y = delta_y - 1, len_x = delta_x - 2;
    if (bar_win == NULL) bar_win = newwin(len_y, len_x, starty, startx);
    else wclear(bar_win);
    int bg = 239;
    init_pair(bg, bg, BLACK);
    wattron(bar_win, COLOR_PAIR(bg));
    for (int i = 1; i < hp_bar_len + 1; i++) {
        mvwprintw(bar_win, 0, i, "\u25A6");
    }
    for (int i = 1; i < stamina_bar_len + 1; i++) {
        mvwprintw(bar_win, 1, i, "\u25A6");
    }
    for (int i = 1; i < enchant_bar_len + 1; i++) {
        mvwprintw(bar_win, 2, i, "\u25A6");
    }
    for (int i = 1; i < enchant_bar_len + 1; i++) {
        mvwprintw(bar_win, 3, i, "\u25A6");
    }
    wattroff(bar_win, COLOR_PAIR(bg));


    int hp_rel = (hp * hp_bar_len + (Max_hp - 1)) / Max_hp;
    int stamina_rel = (stamina * stamina_bar_len + (Max_stamina - 1)) / Max_stamina;
    int enchant_speed_rel = (enchant_speed * enchant_bar_len + (Max_enchant - 1)) / Max_enchant;
    int enchant_damage_rel = (enchant_damage * enchant_bar_len + (Max_enchant - 1)) / Max_enchant;

    int fr = 160;
    init_pair(fr, fr, BLACK);
    wattron(bar_win, COLOR_PAIR(fr) | A_BOLD);

    wattron(bar_win, A_ITALIC);
    mvwprintw(bar_win ,0, hp_bar_len + 3, "%3d%%", (hp * 100 + (Max_hp - 1)) / Max_hp);
    wattroff(bar_win, A_ITALIC);
    for (int i = 1; i < hp_rel + 1; i++) {
        mvwprintw(bar_win, 0, i, "\u25A6");
    }
    wattroff(bar_win, COLOR_PAIR(fr) | A_BOLD);

    fr = 148;
    init_pair(fr, fr, BLACK);
    wattron(bar_win, COLOR_PAIR(fr) | A_BOLD);
    wattron(bar_win, A_ITALIC);
    mvwprintw(bar_win ,1, stamina_bar_len + 3, "%3d%%", (stamina * 100 + (Max_stamina - 1)) / Max_stamina);
    wattroff(bar_win, A_ITALIC);
    for (int i = 1; i < stamina_rel + 1; i++) {
        mvwprintw(bar_win, 1, i, "\u25A6");
    }
    wattroff(bar_win, COLOR_PAIR(fr) | A_BOLD);

    fr = 57;
    init_pair(fr, fr, BLACK);
    wattron(bar_win, COLOR_PAIR(fr) | A_BOLD);
    wattron(bar_win, A_ITALIC);
    mvwprintw(bar_win ,2, enchant_bar_len + 3, "%3d%%", (enchant_speed * 100 + (Max_enchant - 1)) / Max_enchant);
    wattroff(bar_win, A_ITALIC);
    for (int i = 1; i < enchant_speed_rel + 1; i++) {
        mvwprintw(bar_win, 2, i, "\u25A6");
    }
    wattroff(bar_win, COLOR_PAIR(fr) | A_BOLD);

    fr = 126;
    init_pair(fr, fr, BLACK);
    wattron(bar_win, COLOR_PAIR(fr) | A_BOLD);
    wattron(bar_win, A_ITALIC);
    mvwprintw(bar_win ,3, enchant_bar_len + 3, "%3d%%", (enchant_damage * 100 + (Max_enchant - 1)) / Max_enchant);
    wattroff(bar_win, A_ITALIC);
    for (int i = 1; i < enchant_damage_rel + 1; i++) {
        mvwprintw(bar_win, 3, i, "\u25A6");
    }
    wattroff(bar_win, COLOR_PAIR(fr) | A_BOLD);


    wrefresh(bar_win);
}

void refresh_map(map *mp) {
    if (mp_win == NULL) mp_win = newwin(line_lvl, col_lvl, 0, startx_lvl);
    else wclear(mp_win);

    lvl *lv = mp->lvls[mp->curr_lvl];
    //cell and hidden cells;
    for (int i = 0; i < line_lvl; i++) {
        for (int j = 0; j < col_lvl; j++) {
            if (lv->explore_sit[i][j] == 0 && M_mode_on == 0) continue;
            if (lv->cell[i][j] == 0 && lv->hidden_sit[i][j] == 0) continue;
            mvwaddch(mp_win, i, j, lv->cell[i][j]);
            if (lv->hidden_sit[i][j] == 2) {
                mvwaddch(mp_win, i, j, lv->hidden_cell[i][j]);
            }
        }
    }

    //food;
    for (int i = 0; i < line_lvl; i++) {
        for (int j = 0; j < col_lvl; j++) {
            if (lv->explore_sit[i][j] == 0 && M_mode_on == 0) continue;
            if (lv->foods->pickable_sit[i][j]) {
                wattron(mp_win,lv->foods->attr[i][j]);
                mvwprintw(mp_win, i, j, "%s", lv->foods->cells[i][j]);
                wattroff(mp_win,lv->foods->attr[i][j]);
            }
        }
    }

    //golds;
    for (int i = 0; i < line_lvl; i++) {
        for (int j = 0; j < col_lvl; j++) {
            if (lv->explore_sit[i][j] == 0 && M_mode_on == 0) continue;
            if (lv->golds->pickable_sit[i][j]) {
                wattron(mp_win,lv->golds->attr[i][j]);
                mvwprintw(mp_win, i, j, "%s", lv->golds->cells[i][j]);
                wattroff(mp_win,lv->golds->attr[i][j]);
            }
        }
    }

    //talismans;
    for (int i = 0; i < line_lvl; i++) {
        for (int j = 0; j < col_lvl; j++) {
            if (lv->explore_sit[i][j] == 0 && M_mode_on == 0) continue;
            if (lv->talismans->pickable_sit[i][j]) {
                wattron(mp_win,lv->talismans->attr[i][j]);
                mvwprintw(mp_win, i, j, "%s", lv->talismans->cells[i][j]);
                wattroff(mp_win,lv->talismans->attr[i][j]);
            }
        }
    }

    //weapons;
    for (int i = 0; i < line_lvl; i++) {
        for (int j = 0; j < col_lvl; j++) {
            if (lv->explore_sit[i][j] == 0 && M_mode_on == 0) continue;
            if (lv->weapons->pickable_sit[i][j]) {
                wattron(mp_win,lv->weapons->attr[i][j]);
                mvwprintw(mp_win, i, j, "%s", lv->weapons->cells[i][j]);
                wattroff(mp_win,lv->weapons->attr[i][j]);
            }
        }
    }

    //monster;
    for (int i = 0; i < line_lvl; i++) {
        for (int j = 0; j < col_lvl; j++) {
            if (lv->explore_sit[i][j] == 0 && M_mode_on == 0) continue;
            for (int k = 0; k < lv->monster_num; k++) {
                if (lv->monster[k]->cnt[i][j]) {
                    wattron(mp_win,lv->monster[k]->attr);
                    mvwprintw(mp_win, i, j, "%c", lv->monster[k]->look);
                    wattroff(mp_win,lv->monster[k]->attr);
                }
            }
        }
    }

    wattron(mp_win, hero_attr);
    mvwprintw(mp_win, mp->hero_place.y, mp->hero_place.x, "%s", hero);
    wattroff(mp_win, hero_attr);
    wrefresh(mp_win);
}

void get_name_of_item_and_cnt_by_shape_and_attr_in_inv (Inventory *inv,char *shape, chtype attr, int *cnt, char **name) {
    char *res_name = (char *)malloc(30 * sizeof(char));
    res_name[0] = '\0';
    if (!strcmp(shape, gold_regular) && attr == gold_regular_attr) {
        *cnt = inv->gold_cnt;
        strcpy(res_name, "gold");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, gold_black) && attr == gold_black_attr) {
        *cnt = inv->gold_cnt;
        strcpy(res_name, "Black gold");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, food_golden) && attr == food_golden_attr) {
        *cnt = inv->food_golden_cnt;
        strcpy(res_name, "Golden meat");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, food_reg) && attr == food_reg_attr) {
        *cnt = inv->food_reg_cnt;
        strcpy(res_name, "Regular meat");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, food_magical) && attr == food_magical_attr) {
        *cnt = inv->food_magical_cnt;
        strcpy(res_name, "Magical meat");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, food_rotten) && attr == food_rotten_attr) {
        *cnt = inv->food_rotten_cnt;
        strcpy(res_name, "Rotten meat");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, weapon_arrow) && attr == weapon_arrow_attr) {
        *cnt = inv->weapon_arrow_cnt;
        strcpy(res_name, "Arrow");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, weapon_dagger) && attr == weapon_dagger_attr) {
        *cnt = inv->weapon_dagger_cnt;
        strcpy(res_name, "Dagger");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, weapon_sword) && attr == weapon_sword_attr) {
        *cnt = inv->weapon_sword_cnt;
        strcpy(res_name, "Sword");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, weapon_mace) && attr == weapon_mace_attr) {
        *cnt = inv->weapon_mace_cnt;
        strcpy(res_name, "Mace");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, weapon_magic_wand) && attr == weapon_magic_wand_attr) {
        *cnt = inv->weapon_magic_wand_cnt;
        strcpy(res_name, "Magical Wand");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, talisman_damage) && attr == talisman_damage_attr) {
        *cnt = inv->talisman_damage_cnt;
        strcpy(res_name, "Power");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, talisman_health) && attr == talisman_health_attr) {
        *cnt = inv->talisman_health_cnt;
        strcpy(res_name, "Health");
        *name = res_name;
        return;
    }
    if (!strcmp(shape, talisman_speed) && attr == talisman_speed_attr) {
        *cnt = inv->talisman_speed_cnt;
        strcpy(res_name, "Speed");
        *name = res_name;
        return;
    }
    
}

void refresh_info_box (map *mp, user *player) {
    int starty = delta_y + 1, startx = 1, len_y = LINES - 1 - delta_y - 1, len_x = delta_x - 2;
    if (info_win == NULL) info_win = newwin(len_y, len_x, starty, startx);
    else wclear(info_win);
    
    for (int i = 0; i < delta_x - 1; i++) {
        mvwaddch(info_win, len_y - delta_y + 1, i, ACS_HLINE | COLOR_PAIR(BLUE_ON_BLACK));
    }

    char room_name[30]; room_name[0] = '\0';
    chtype attr;
    int rm_id =  mp->lvls[mp->curr_lvl]->room_id[mp->hero_place.y][mp->hero_place.x];
    if (rm_id == mp->lvls[mp->curr_lvl]->enchant_room_id) {
        strcpy(room_name, "Enchant room");
        attr = COLOR_PAIR(BLUE_D_ON_BLACK);
    } else if (rm_id == mp->lvls[mp->curr_lvl]->nightmare_room_id) {
        strcpy(room_name, "Nightmare room");
        attr = COLOR_PAIR(PURPLE_D_ON_BLACK);
    } else if (rm_id == mp->lvls[mp->curr_lvl]->treasure_room_id) {
        strcpy(room_name, "Treasure room"); 
        attr = COLOR_PAIR(YELLOW_ON_BLACK);
    } else if (rm_id >= 0) {
        strcpy(room_name, "Regular room");
        attr = COLOR_PAIR(RED_ON_BLACK);
    } else {
        strcpy(room_name, "Crossroad");
        attr = COLOR_PAIR(GRAY_ON_BLACK);
    }

    wattron(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(info_win, len_y - delta_y + 2, 1, "Room name: ");
    wattroff(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    wattron(info_win, A_ITALIC | attr);
    wprintw(info_win, "%s", room_name);
    wattroff(info_win, A_ITALIC | attr);

    wattron(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(info_win, len_y - delta_y + 3, 1, "level: ");
    wattroff(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    wattron(info_win, A_ITALIC | COLOR_PAIR(RED_ON_BLACK));
    if (mp->curr_lvl < mp->lvl_num - 1) wprintw(info_win, "%d", mp->curr_lvl + 1);
    else wprintw(info_win, "%d (last level)", mp->curr_lvl + 1);
    wattroff(info_win, A_ITALIC | COLOR_PAIR(RED_ON_BLACK));

    wattron(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(info_win, len_y - delta_y + 4, 1, "Username: ");
    wattroff(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    wattron(info_win, A_ITALIC | COLOR_PAIR(RED_ON_BLACK));
    wprintw(info_win, "%s", player->username);
    wattroff(info_win, A_ITALIC | COLOR_PAIR(RED_ON_BLACK));


    //
    wattron(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(info_win, 1, 1, "Golds: ");
    wattroff(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));

    wattron(info_win,COLOR_PAIR(YELLOW_ON_BLACK) | A_ITALIC);
    wprintw(info_win, "%d", mp->inv->gold_cnt);
    wattroff(info_win,COLOR_PAIR(YELLOW_ON_BLACK) | A_ITALIC);

    wattron(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(info_win, 2, 1, "Weapon: ");
    wattroff(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));

    int cnt; char *full_name;
    get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, mp->inv->weapon_def, mp->inv->weapon_def_attr, &cnt, &full_name);
    wattron(info_win,mp->inv->weapon_def_attr | A_ITALIC);
    wprintw(info_win, "%s %s (%d)", mp->inv->weapon_def, full_name, cnt);
    wattroff(info_win,mp->inv->weapon_def_attr | A_ITALIC);

    wattron(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(info_win, 3, 1, "Talisman: ");
    wattroff(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));

    get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, mp->inv->talisman_def, mp->inv->talisman_def_attr, &cnt, &full_name);
    wattron(info_win,mp->inv->talisman_def_attr | A_ITALIC);
    wprintw(info_win, "%s %s (%d)", mp->inv->talisman_def, full_name, cnt);
    wattroff(info_win,mp->inv->talisman_def_attr | A_ITALIC);

    wattron(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));
    mvwprintw(info_win, 4, 1, "Food: ");
    wattroff(info_win,A_BOLD | COLOR_PAIR(GRAY_ON_BLACK));

    get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, mp->inv->food_def, mp->inv->food_def_attr, &cnt, &full_name);
    wattron(info_win,mp->inv->food_def_attr | A_ITALIC);
    wprintw(info_win, "%s %s (%d)", mp->inv->food_def, full_name, cnt);
    wattroff(info_win,mp->inv->food_def_attr | A_ITALIC);

    wrefresh(info_win);
}

void refresh_game (map *mp, user *player) {
    mp->hp = hp;
    mp->stamina = stamina;
    mp->enchant_damage = enchant_damage;
    mp->enchant_speed = enchant_speed;
    power_bar_show();
    refresh_map(mp);
    refresh_info_box(mp, player);
}

void add_this_to_inv (map *mp, char *shape, chtype attr, int cnt) {
    Inventory *inv = mp->inv;
    if (!strcmp(shape, gold_regular) && attr == gold_regular_attr) {
        inv->gold_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" gold.");

        return;
    }
    if (!strcmp(shape, gold_black) && attr == gold_black_attr) {
        inv->gold_cnt += cnt * 5;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Black gold.");
        return;
    }
    if (!strcmp(shape, food_golden) && attr == food_golden_attr) {
        inv->food_golden_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Golden meat.");
        return;
    }
    if (!strcmp(shape, food_reg) && attr == food_reg_attr) {
        inv->food_reg_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Regular meat.");
        return;
    }
    if (!strcmp(shape, food_magical) && attr == food_magical_attr) {
        inv->food_magical_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Magical meat.");
        return;
    }
    if (!strcmp(shape, food_rotten) && attr == food_rotten_attr) {
        inv->food_rotten_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Rotten meat.");
        return;
    }
    if (!strcmp(shape, weapon_arrow) && attr == weapon_arrow_attr) {
        inv->weapon_arrow_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Arrow.");
        return;
    }
    if (!strcmp(shape, weapon_dagger) && attr == weapon_dagger_attr) {
        inv->weapon_dagger_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Dagger.");
        return;
    }
    if (!strcmp(shape, weapon_sword) && attr == weapon_sword_attr) {
        inv->weapon_sword_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Sword.");
        return;
    }
    if (!strcmp(shape, weapon_mace) && attr == weapon_mace_attr) {
        inv->weapon_mace_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Mace.");
        return;
    }
    if (!strcmp(shape, weapon_magic_wand) && attr == weapon_magic_wand_attr) {
        inv->weapon_magic_wand_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Magical wand.");
        return;
    }
    if (!strcmp(shape, talisman_damage) && attr == talisman_damage_attr) {
        inv->talisman_damage_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Damage talisman.");
        return;
    }
    if (!strcmp(shape, talisman_health) && attr == talisman_health_attr) {
        inv->talisman_health_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Health talisman.");
        return;
    }
    if (!strcmp(shape, talisman_speed) && attr == talisman_speed_attr) {
        inv->talisman_speed_cnt += cnt;
        show_message_cover("Added to Inventory: ", 0);
        show_message_continue_int(cnt);
        show_message_continue(" Speed talisman.");
        return;
    }
}

void pick_pos(map *mp, int y, int x) {
    lvl *lv = mp->lvls[mp->curr_lvl];
    if(lv->weapons->pickable_sit[y][x]) {
        pickable_things *pk = lv->weapons;
        pk->pickable_sit[y][x] = 0;
        add_this_to_inv(mp, pk->cells[y][x], pk->attr[y][x], pk->cnt[y][x]);
        pk->cells[y][x][0] = '\0';
        pk->attr[y][x] = 0;
        pk->cnt[y][x] = 0;
        
    }
    if(lv->talismans->pickable_sit[y][x]) {
        pickable_things *pk = lv->talismans;
        pk->pickable_sit[y][x] = 0;
        add_this_to_inv(mp, pk->cells[y][x], pk->attr[y][x], pk->cnt[y][x]);
        pk->cells[y][x][0] = '\0';
        pk->attr[y][x] = 0;
        pk->cnt[y][x] = 0;
        
    }
    if(lv->golds->pickable_sit[y][x]) {
        pickable_things *pk = lv->golds;
        pk->pickable_sit[y][x] = 0;
        add_this_to_inv(mp, pk->cells[y][x], pk->attr[y][x], pk->cnt[y][x]);
        pk->cells[y][x][0] = '\0';
        pk->attr[y][x] = 0;
        pk->cnt[y][x] = 0;
    }
    if(lv->foods->pickable_sit[y][x]) {
        pickable_things *pk = lv->foods;
        pk->pickable_sit[y][x] = 0;
        add_this_to_inv(mp, pk->cells[y][x], pk->attr[y][x], pk->cnt[y][x]);
        pk->cells[y][x][0] = '\0';
        pk->attr[y][x] = 0;
        pk->cnt[y][x] = 0;
    }
}

int move_one (map *mp, elmnt dest, int do_pick) { //0 cant move this way, 1 moved;
    lvl *lv = mp->lvls[mp->curr_lvl];
    int y = dest.y, x = dest.x;
    if (lv->cell[y][x] == 0) return 0;
    if (lv->cell[mp->hero_place.y][mp->hero_place.x] == el_window && lv->cell[y][x] == el_road) {
        show_message_cover("You cant jump from window!", 0);
        return 0;
    }
    if (lv->cell[mp->hero_place.y][mp->hero_place.x] == el_road && lv->cell[y][x] == el_window) {
        show_message_cover("You cant go through window!", 0);
        return 0;
    }
    for (int i = 0; i < lv->monster_num; i++) {
        if (lv->monster[i]->cnt[y][x]) {
            show_message_cover("Wanna die?!", 0);
            return 0;
        }
    }
    if (is_wall(lv->cell[y][x])) {
        if (lv->hidden_sit[y][x] == 0) {
            return 0;
        } else if (lv->hidden_cell[y][x] == el_hidden_door) {
            lv->hidden_sit[y][x] = 2;
            
        }
    }

    if (do_pick) {
        pick_pos(mp, y, x);
    }
    
    if (lv->cell[y][x] == el_trap) {
        hp -= 5;
        show_message_cover("Oops! you walked on a trap.", 0);
    }
    mp->hero_place.y = dest.y;
    mp->hero_place.x = dest.x;
    if (lv->hidden_sit[y + 1][x] == 1) {
        if (lv->room_id[y][x] == lv->room_id[y + 1][x] || lv->room_id[y + 1][x] >= lv->reg_room_num)
            lv->hidden_sit[y + 1][x] = 2;
    }
    if (lv->hidden_sit[y - 1][x] == 1) {
        if (lv->room_id[y][x] == lv->room_id[y - 1][x] || lv->room_id[y - 1][x] >= lv->reg_room_num)
            lv->hidden_sit[y - 1][x] = 2;
    }
    if (lv->hidden_sit[y][x + 1] == 1) {
        if (lv->room_id[y][x] == lv->room_id[y][x + 1] || lv->room_id[y][x + 1] >= lv->reg_room_num)
            lv->hidden_sit[y][x + 1] = 2;
    }
    if (lv->hidden_sit[y][x - 1] == 1) {
        if (lv->room_id[y][x] == lv->room_id[y][x - 1] || lv->room_id[y][x - 1] >= lv->reg_room_num)
            lv->hidden_sit[y][x - 1] = 2;
    }

    int src = lv->room_id[y][x];
    if (src != lv->nightmare_room_id && src >= 0) {
        for (int i = lv->rooms[src]->starty; i < lv->rooms[src]->starty + lv->rooms[src]->h; i++) {
            for (int j = lv->rooms[src]->startx; j < lv->rooms[src]->startx + lv->rooms[src]->w; j++) {
                lv->explore_sit[i][j] = 1;
            }
        }
    }

    int v, u;
    for (int s = -2; s <= 2; s++) {
        for (int t = -2; t <= 2; t++) {
            v = y + s;
            u = x + t;
            if (v <= 0 || v >= line_lvl - 1) continue;
            if (u <= 0 || u >= col_lvl - 1) continue;
            if (lv->explore_sit[v][u] == 1) continue;
            if (lv->cell[y][x] == el_road) {
                if (lv->cell[v][u] == el_road) lv->explore_sit[v][u] = 1;
                else if (is_wall(lv->cell[v][u]) || lv->cell[v][u] == el_door || lv->cell[v][u] == el_window) lv->explore_sit[v][u] = 1;
            } else {
                int k = lv->room_id[y][x]; //nightmare;
                if (lv->hidden_cell[y][x] == el_hidden_door && k == lv->nightmare_room_id) {
                    
                    for (int i = lv->rooms[k]->starty; i < lv->rooms[k]->starty + lv->rooms[k]->h; i++) {
                        lv->explore_sit[i][lv->rooms[k]->startx] = 1;
                        lv->explore_sit[i][lv->rooms[k]->startx + lv->rooms[k]->w - 1] = 1;
                    }
                    for (int j = lv->rooms[k]->startx; j < lv->rooms[k]->startx + lv->rooms[k]->w; j++) {
                        lv->explore_sit[lv->rooms[k]->starty][j] = 1;
                        lv->explore_sit[lv->rooms[k]->starty + lv->rooms[k]->h - 1][j] = 1; 
                    }
                }
                lv->explore_sit[v][u] = 1;
            }
        }
    }

    return 1;

}

int move_player(map *mp, int way_ch, int is_force_one, int do_pick) { //speed talisman +1, shift and char(uppercase) +1, //'-'before all dont pick
    int d = 1;
    if (way_ch >= 'A' && way_ch <= 'Z') {
        d = 2;
        way_ch = way_ch - 'A' + 'a';
    }
    if (enchant_speed > 0) ++d;
    if (is_force_one) d = 1;
    int del_y[8], del_x[8];
    /*
        q w e
        a   d
        z s c

        4 0 5
        3   1
        7 2 6
    */
    for (int i = 0; i < 8; i++) {
        del_x[i] = 0;
        del_y[i] = 0;
    } 
    del_y[0] = del_y[4] = del_y[5] = -1;
    del_y[2] = del_y[7] = del_y[6] = 1;
    del_x[5] = del_x[1] = del_x[6] = 1;
    del_x[4] = del_x[3] = del_x[7] = -1;

   
    int wy;
    switch (way_ch) {
        case 'w' : wy = 0; break;
        case 'e' : wy = 5; break;
        case 'd' : wy = 1; break;
        case 'c' : wy = 6; break;
        case 's' : wy = 2; break;
        case 'z' : wy = 7; break;
        case 'a' : wy = 3; break;
        case 'q' : wy = 4; break;
        default : return 0;
    }
    
    
    elmnt dest; dest.y = mp->hero_place.y; dest.x = mp->hero_place.x;
    int did = 0;
    for (int i = 1; i <= d; i++) {
        dest.y += del_y[wy];
        dest.x += del_x[wy];
        int t = move_one(mp, dest, do_pick);
        did += t;
        if (t == 0) break;
    }
    if (did) return 1;
    return 0;

}

void select_item(WINDOW *win, char *categ, map *mp, user *player) {
    char option[10][30];
    char *item[10];
    chtype attr[10];
    int op_num;
    int selected = 0;
    char **what_should_change;
    chtype *attr_what_chould_change;
    if (!strcmp(categ, "Weapons")) {
        op_num = 5;
        int cnt;
        char *name;
        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, weapon_sword, weapon_sword_attr, &cnt, &name);
        snprintf(option[0], sizeof(option[0]), "%s %s (%d)", weapon_sword, name, cnt);
        attr[0] = weapon_sword_attr;
        item[0] = weapon_sword;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, weapon_dagger, weapon_dagger_attr, &cnt, &name);
        snprintf(option[1], sizeof(option[1]), "%s %s (%d)", weapon_dagger, name, cnt);
        attr[1] = weapon_dagger_attr;
        item[1] = weapon_dagger;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, weapon_magic_wand, weapon_magic_wand_attr, &cnt, &name);
        snprintf(option[2], sizeof(option[2]), "%s %s (%d)", weapon_magic_wand, name, cnt);
        attr[2] = weapon_magic_wand_attr;
        item[2] = weapon_magic_wand;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, weapon_arrow, weapon_arrow_attr, &cnt, &name);
        snprintf(option[3], sizeof(option[3]), "%s %s (%d)", weapon_arrow, name, cnt);
        attr[3] = weapon_arrow_attr;
        item[3] = weapon_arrow;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, weapon_mace, weapon_mace_attr, &cnt, &name);
        snprintf(option[4], sizeof(option[4]), "%s %s (%d)", weapon_mace, name, cnt);
        attr[4] = weapon_mace_attr;
        item[4] = weapon_mace;

        what_should_change = &mp->inv->weapon_def;
        attr_what_chould_change = &mp->inv->weapon_def_attr;
    }
    if (!strcmp(categ, "Talismans")) {
        op_num = 3;
        int cnt;
        char *name;
        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, talisman_health, talisman_health_attr, &cnt, &name);
        snprintf(option[0], sizeof(option[0]), "%s %s (%d)", talisman_health, name, cnt);
        attr[0] = talisman_health_attr;
        item[0] = talisman_health;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, talisman_damage, talisman_damage_attr, &cnt, &name);
        snprintf(option[1], sizeof(option[1]), "%s %s (%d)", talisman_damage, name, cnt);
        attr[1] = talisman_damage_attr;
        item[1] = talisman_damage;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, talisman_speed, talisman_speed_attr, &cnt, &name);
        snprintf(option[2], sizeof(option[2]), "%s %s (%d)", talisman_speed, name, cnt);
        attr[2] = talisman_speed_attr;
        item[2] = talisman_speed;

        what_should_change = &mp->inv->talisman_def;
        attr_what_chould_change = &mp->inv->talisman_def_attr;
    }
    if (!strcmp(categ, "Foods")) {
        op_num = 4;
        int cnt;
        char *name;
        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, food_reg, food_reg_attr, &cnt, &name);
        snprintf(option[0], sizeof(option[0]), "%s %s (%d)", food_reg, name, cnt);
        attr[0] = food_reg_attr;
        item[0] = food_reg;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, food_golden, food_golden_attr, &cnt, &name);
        snprintf(option[1], sizeof(option[1]), "%s %s (%d)", food_golden, name, cnt);
        attr[1] = food_golden_attr;
        item[1] = food_golden;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, food_magical, food_magical_attr, &cnt, &name);
        snprintf(option[2], sizeof(option[2]), "%s %s (%d)", food_magical, name, cnt);
        attr[2] = food_magical_attr;
        item[2] = food_magical;

        get_name_of_item_and_cnt_by_shape_and_attr_in_inv(mp->inv, food_rotten, food_rotten_attr, &cnt, &name);
        snprintf(option[3], sizeof(option[3]), "%s %s (%d)", food_rotten, name, cnt);
        attr[3] = food_rotten_attr;
        item[3] = food_rotten;

        what_should_change = &mp->inv->food_def;
        attr_what_chould_change = &mp->inv->food_def_attr;
    }

    int ch;
    do {
        for (int i = 0; i < op_num; i++) {
            if (i == selected) {
                wattron(win, attr[i] | A_REVERSE);
                mvwprintw(win, i + 1, 1, "%s", option[i]);
                wattroff(win, attr[i] | A_REVERSE);
            } else {
                wattron(win, attr[i]);
                mvwprintw(win, i + 1, 1, "%s", option[i]);
                wattroff(win, attr[i]);
            }
        }
        wrefresh(win);
        ch = getch();
        switch (ch) {
            case KEY_UP :
                selected += op_num;
                --selected;
                selected %= op_num;
                break;
            case KEY_DOWN :
                ++selected;
                selected %= op_num;
                break;
            case KEY_RIGHT :
                *what_should_change = item[selected];
                *attr_what_chould_change = attr[selected];
                show_message_cover("Default items updated.", 0);
                break;
            case KEY_LEFT :
                break;
            default :
                show_message_cover("UP and DOWN key for changing option, LEFT key for back, RIGHT key for select.", 0);
        }
    } while (ch != KEY_LEFT);
    wclear(win);

}

void open_inventory (map *mp, user *player) {
    int starty = delta_y + 1, startx = 1, len_y = LINES - 1 - delta_y - 1, len_x = delta_x - 2;
    len_y = len_y - delta_y + 1;

    WINDOW *win = newwin(len_y, len_x, starty, startx);
    char *option[] = {"Weapons", "Talismans", "Foods"};
    chtype attr[] = {A_BOLD | COLOR_PAIR(GRAY_ON_BLACK), A_BOLD | COLOR_PAIR(GRAY_ON_BLACK), A_BOLD | COLOR_PAIR(GRAY_ON_BLACK)};
    int op_num = 3;
    int selected = 0;
    int ch;
    do {
        for (int i = 0; i < op_num; i++) {
            if (i == selected) {
                wattron(win, attr[i] | A_REVERSE);
                mvwprintw(win, i + 1, 1, "%s", option[i]);
                wattroff(win, attr[i] | A_REVERSE);
            } else {
                wattron(win, attr[i]);
                mvwprintw(win, i + 1, 1, "%s", option[i]);
                wattroff(win, attr[i]);
            }
        }
        wrefresh(win);
        ch = getch();
        switch (ch) {
            case KEY_UP :
                selected += op_num;
                --selected;
                selected %= op_num;
                break;
            case KEY_DOWN :
                ++selected;
                selected %= op_num;
                break;
            case KEY_RIGHT :
                select_item(win, option[selected], mp, player);
                break;
            case KEY_LEFT :
                break;
            default :
                show_message_cover("UP and DOWN key for changing option, LEFT key for back, RIGHT key for select.", 0);
        }
    } while (ch != KEY_LEFT);
    wclear(win);

}

int adjance (int y1, int x1, int y2, int x2) {
    int h = y1 - y2;
    int w = x1 - x2;
    if (h < 0) h *= -1;
    if (w < 0) w *= -1;
    if ((h + w) == 1) return 1;
    return 0;
}

void move_enemy (map *mp, user *player, int mon_id) {
    lvl *lv = mp->lvls[mp->curr_lvl];
    int hy = mp->hero_place.y, hx = mp->hero_place.x;
    int rm_id = lv->room_id[hy][hx];
    if (!(rm_id >= 0 && rm_id < lv->room_num)) return;
    int sty = lv->rooms[rm_id]->starty;
    int stx = lv->rooms[rm_id]->startx;
    int len_y = lv->rooms[rm_id]->h;
    int len_x = lv->rooms[rm_id]->w;

    
    enemy *mon = lv->monster[mon_id];
    int cnt[700][700], hp[700][700], token[700][700];
    for (int i = sty; i < sty + len_y; i++) {
        for (int j = stx; j < stx + len_x; j++) {
            cnt[i][j] = 0;
            hp[i][j] = 0;
            token[i][j] = 0;
        }
    }
    for (int i = sty; i < len_y + sty; i++) {
        for (int j = stx; j < len_x + stx; j++) {
            if (mon->cnt[i][j] == 0) continue;
            if (adjance(i, j, hy, hx)) {
                if (mon->token[i][j] < mon->max_token) token[i][j] += mon->max_token;
                else token[i][j] += mon->token[i][j];
                cnt[i][j] += 1;
                hp[i][j] += mon->hp[i][j];
            } else {
                if (mon->token[i][j] == 0) {
                    hp[i][j] += mon->hp[i][j];
                    cnt[i][j] += 1;
                    continue;
                }
                
                int y = i, x = j;
                if (i < hy) ++y;
                else if (i > hy) --y;
                if (j < hx) ++x;
                else if (j > hx) --x;
                if (y == hy && x == hx) {
                    if (!(lv->cell[i][x] == 0 || lv->cell[i][x] == el_door || is_wall(lv->cell[i][x]) || lv->hidden_sit[i][x] || lv->cell[i][x] == el_window)) {
                        y = i;
                    } else x = j;                                           
                }
                if (lv->cell[y][x] == 0 || lv->cell[y][x] == el_door || is_wall(lv->cell[y][x]) || lv->hidden_sit[y][x] || lv->cell[y][x] == el_window) {
                    token[i][j] += mon->token[i][j];
                    cnt[i][j] += 1;
                    hp[i][j] += mon->hp[i][j];
                    continue;
                }
                cnt[y][x] += 1;
                hp[y][x] += mon->hp[i][j];
                token[y][x] += mon->token[i][j];

            }
        }
    }
    
    for (int i = sty; i < sty + len_y; i++) {
        for (int j = stx; j < stx + len_x; j++) {
            if (cnt[i][j]) cnt[i][j] = 1;
            mon->cnt[i][j] = cnt[i][j];
            mon->token[i][j] = token[i][j];
            mon->hp[i][j] = hp[i][j];
        }
    }

}

void enemy_attack (map *mp, user *player, int mon_id) {
    lvl *lv = mp->lvls[mp->curr_lvl];
    int hy = mp->hero_place.y, hx = mp->hero_place.x;
    int rm_id = lv->room_id[hy][hx];
    if (!(rm_id >= 0 && rm_id < lv->room_num)) return;
    int sty = lv->rooms[rm_id]->starty;
    int stx = lv->rooms[rm_id]->startx;
    int len_y = lv->rooms[rm_id]->h;
    int len_x = lv->rooms[rm_id]->w;
    
    enemy *mon = lv->monster[mon_id];
    for (int i = sty; i < sty + len_y; i++) {
        for (int j = stx; j < stx + len_x; j++) {
            if (mon->cnt[i][j] && adjance(i, j, hy, hx)) {
                hp -= mon->damage;
                if (player->difficulty == dif_hard) hp -= mon->damage;
                if (player->difficulty == dif_easy) hp += 5;
                show_message_cover("A critical hit from ", 0);
                switch(mon_id) {
                    case 0 : show_message_continue("Deamon."); break;
                    case 1 : show_message_continue("Fire Breathing Monster.");break;
                    case 2 : show_message_continue("Giant."); break;
                    case 3 : show_message_continue("Snake."); break;
                    case 4 : show_message_continue("Undead."); break;
                    default : show_message_continue("GOD."); break;
                }
            }
        }
    }
        
    
}

void now_its_enemy_turn (map *mp, user *player) {
    move_enemy(mp, player, 0);
    enemy_attack(mp, player, 0);
    move_enemy(mp, player, 1);
    enemy_attack(mp, player, 1);
    if (dif_normal == player->difficulty) {
        enemy_attack(mp, player, 2);
        move_enemy(mp, player, 2);

        enemy_attack(mp, player, 3);
        move_enemy(mp, player, 3);

        move_enemy(mp, player, 4);
        enemy_attack(mp, player, 4);
    
    } else if (dif_hard == player->difficulty) {
        move_enemy(mp, player, 2);
        enemy_attack(mp, player, 2);

        move_enemy(mp, player, 3);
        enemy_attack(mp, player, 3);

        move_enemy(mp, player, 4);
        enemy_attack(mp, player, 4);
    } else {
        enemy_attack(mp, player, 2);
        move_enemy(mp, player, 2);

        enemy_attack(mp, player, 3);
        move_enemy(mp, player, 3);

        enemy_attack(mp, player, 4);
        move_enemy(mp, player, 4);
       
    }
}

void play_with_user (map *mp, user * player) {
    hp = mp->hp;
    stamina = mp->stamina;
    enchant_damage = mp->enchant_damage;
    enchant_speed = mp->enchant_speed;
    refresh_game(mp, player);
    int ch;
    int do_pick = 1;
    int is_force_on = 0;
    int stamina_circle = 20;
    
    while ((ch = getch()) != KEY_F(1)) {
        int did = 0;
        /*
        q w e
        a   d
        z s c
        capital = speed +1;
        '=' is on --> force one move
        '-' is on --> don't pick any
        'p' --> pick items in your current position;
        'i' --> open inventory;
        'T' --> use talisamn;
        'F' --> eat food;
        */
        lvl *lv = mp->lvls[mp->curr_lvl];
        int y = mp->hero_place.y;
        int x = mp->hero_place.x;
        if (ch == '-') {
            do_pick ^= 1;
        } else if (ch == '=') {
            is_force_on ^= 1;
        } else if (ch == 'p') {
            pick_pos(mp, mp->hero_place.y, mp->hero_place.x);
            show_message_cover("Items on the ground picked up.", 0);
            did = 1;
        } else if (ch == KEY_DOWN) {
            if (lv->cell[y][x] == el_stair) {
                mp->curr_lvl += 1;
                mp->hero_place.y = mp->lvls[mp->curr_lvl]->up_stair.y;
                mp->hero_place.x = mp->lvls[mp->curr_lvl]->up_stair.x;
                show_message_cover("Next level!", 0);
                did = 1;
            }
        } else if (ch == KEY_UP) {
            if (lv->cell[y][x] == el_stair_up) {
                mp->curr_lvl -= 1;
                mp->hero_place.y = mp->lvls[mp->curr_lvl]->down_stair.y;
                mp->hero_place.x = mp->lvls[mp->curr_lvl]->down_stair.x;
                show_message_cover("You've back!", 0);
                did = 1;
            }
        } else if (ch == 'i') {
            open_inventory(mp, player);
        } else if (ch == 'F') {
            if (!strcmp(mp->inv->food_def, food_reg) && mp->inv->food_def_attr == food_reg_attr) {
                if(mp->inv->food_reg_cnt) mp->inv->food_reg_cnt -= 1;
                else show_message_cover("Nothing left.", 0);
                stamina += Max_stamina / 5;
            }
            if (!strcmp(mp->inv->food_def, food_golden) && mp->inv->food_def_attr == food_golden_attr) {
                if(mp->inv->food_golden_cnt) mp->inv->food_golden_cnt -= 1;
                else show_message_cover("Nothing left.", 0);
                stamina += Max_stamina / 2;
                enchant_damage += Max_enchant / 2;
            }
            if (!strcmp(mp->inv->food_def, food_magical) && mp->inv->food_def_attr == food_magical_attr) {
                if(mp->inv->food_magical_cnt) mp->inv->food_magical_cnt -= 1;
                else show_message_cover("Nothing left.", 0);
                stamina += Max_stamina / 5;
                enchant_speed += Max_enchant / 2;
            }
            if (!strcmp(mp->inv->food_def, food_rotten) && mp->inv->food_def_attr == food_rotten_attr) {
                if(mp->inv->food_magical_cnt) mp->inv->food_magical_cnt -= 1;
                else show_message_cover("Nothing left.", 0);
                stamina += Max_stamina / 10;
                hp -= Max_hp / 20;  
            }
        } else if (ch == 'T') {
            if (!strcmp(mp->inv->talisman_def, talisman_health) && mp->inv->talisman_def_attr == talisman_health_attr) {
                if(mp->inv->talisman_health_cnt) mp->inv->talisman_health_cnt -= 1;
                else show_message_cover("Nothing left.", 0);
                hp += Max_hp / 2;
            }
            if (!strcmp(mp->inv->talisman_def, talisman_damage) && mp->inv->talisman_def_attr == talisman_damage_attr) {
                if(mp->inv->talisman_damage_cnt) mp->inv->talisman_damage_cnt -= 1;
                else show_message_cover("Nothing left.", 0);
                enchant_damage = Max_enchant;
            }
            if (!strcmp(mp->inv->talisman_def, talisman_speed) && mp->inv->talisman_def_attr == talisman_speed_attr) {
                if(mp->inv->talisman_speed_cnt) mp->inv->talisman_speed_cnt -= 1;
                else show_message_cover("Nothing left.", 0);
                enchant_speed = Max_enchant;
            }
        } else if (ch == ' ') {
            int del_y[8], del_x[8];
            /*
                q w e
                a   d
                z s c

                4 0 5
                3   1
                7 2 6
            */
            for (int i = 0; i < 8; i++) {
                del_x[i] = 0;
                del_y[i] = 0;
            } 
            del_y[0] = del_y[4] = del_y[5] = -1;
            del_y[2] = del_y[7] = del_y[6] = 1;
            del_x[5] = del_x[1] = del_x[6] = 1;
            del_x[4] = del_x[3] = del_x[7] = -1;

            if (!strcmp(mp->inv->weapon_def, weapon_mace) && mp->inv->weapon_def_attr == weapon_mace_attr) {
                int dmg = 5;
                if (enchant_damage) dmg *= 2;
                show_message_cover("BOOM!", 0);
                for (int i = 0; i < 8; i++) {
                    int y = mp->hero_place.y + del_y[i];
                    int x = mp->hero_place.x + del_x[i];
                    if (lv->room_id[y][x] != lv->room_id[mp->hero_place.y][mp->hero_place.x]) continue;
                    for (int j = 0; j < lv->monster_num; j++) {
                        if (lv->monster[j]->cnt[y][x]) {
                            lv->monster[j]->hp[y][x] -= dmg;
                            lv->monster[j]->token[y][x] = lv->monster[j]->max_token;
                            if (lv->monster[j]->hp[y][x] <= 0) {
                                lv->monster[j]->hp[y][x] = 0;
                                lv->monster[j]->cnt[y][x] = 0;
                                show_message_cover("It's dead now.", 0);
                                switch(lv->monster[j]->look) {
                                            case 'D' :mp->inv->gold_cnt += 5; break;
                                            case 'F' :mp->inv->gold_cnt += 7; break;
                                            case 'G' :mp->inv->gold_cnt += 15 + (10 * (player->difficulty / 2)); break;
                                            case 'S' :mp->inv->gold_cnt += 20 + (15 * (player->difficulty / 2)); break;
                                            case 'U' :mp->inv->gold_cnt += 30 + (15 * (player->difficulty / 2)); break;
                                            default : break;
                                        }
                            }
                        }
                    }
                }
            }
            if (!strcmp(mp->inv->weapon_def, weapon_sword) && mp->inv->weapon_def_attr == weapon_sword_attr) {
                int dmg = 10;
                if (enchant_damage) dmg *= 2;
                if (!mp->inv->weapon_sword_cnt) {
                    show_message_cover("No Sword. HAH!", 0);
                } else {
                    show_message_cover("AAAAAAA!", 0);
                    for (int i = 0; i < 8; i++) {
                        int y = mp->hero_place.y + del_y[i];
                        int x = mp->hero_place.x + del_x[i];
                        if (lv->room_id[y][x] != lv->room_id[mp->hero_place.y][mp->hero_place.x]) continue;
                        for (int j = 0; j < lv->monster_num; j++) {
                            if (lv->monster[j]->cnt[y][x]) {
                                lv->monster[j]->hp[y][x] -= dmg;
                                lv->monster[j]->token[y][x] = lv->monster[j]->max_token;
                                if (lv->monster[j]->hp[y][x] <= 0) {
                                    lv->monster[j]->hp[y][x] = 0;
                                    lv->monster[j]->cnt[y][x] = 0;
                                    show_message_cover("It's dead now.", 0);
                                    switch(lv->monster[j]->look) {
                                            case 'D' :mp->inv->gold_cnt += 5; break;
                                            case 'F' :mp->inv->gold_cnt += 7; break;
                                            case 'G' :mp->inv->gold_cnt += 15 + (10 * (player->difficulty / 2)); break;
                                            case 'S' :mp->inv->gold_cnt += 20 + (15 * (player->difficulty / 2)); break;
                                            case 'U' :mp->inv->gold_cnt += 30 + (15 * (player->difficulty / 2)); break;
                                            default : break;
                                        }
                                }
                            }
                        }
                    }
                    
                }
            }
            if (!strcmp(mp->inv->weapon_def, weapon_dagger) && mp->inv->weapon_def_attr == weapon_dagger_attr) {
                int dmg = 12;
                if (enchant_damage) dmg *= 2;
                int range = 5;
                if (!mp->inv->weapon_dagger_cnt) {
                    show_message_cover("Dagger?...Nope!", 0);
                } else {
                    show_message_cover("Which way?", 0);
                    int ch = getch();
                    int wy = -1;
                    switch (ch) {
                        case 'w' : wy = 0; break;
                        case 'e' : wy = 5; break;
                        case 'd' : wy = 1; break;
                        case 'c' : wy = 6; break;
                        case 's' : wy = 2; break;
                        case 'z' : wy = 7; break;
                        case 'a' : wy = 3; break;
                        case 'q' : wy = 4; break;
                        default :
                            show_message_cover("ha?", 0);
                            break;
                    }
                    if (wy != -1) {
                        mp->inv->weapon_dagger_cnt -= 1;
                        int y = mp->hero_place.y; int x = mp->hero_place.x;
                        int ct = 1;
                        for (int i = 1; i <= range; i++) {
                            int y2 = y + del_y[wy];
                            int x2 = x + del_x[wy];
                            if (lv->cell[y2][x2] == 0) break;
                            if (lv->cell[y2][x2] == el_window) {
                                break;
                            }
                            if (lv->cell[y2][x2] == el_door || lv->hidden_sit[y2][x2]) {
                                break;
                            }
                            if (is_wall(lv->cell[y2][x2])) {
                                break;
                            }
                            for (int j = lv->monster_num - 1; j >= 0; --j) {
                                if (lv->monster[j]->cnt[y2][x2]) {
                                    lv->monster[j]->hp[y2][x2] -= dmg;
                                    lv->monster[j]->token[y][x] = lv->monster[j]->max_token;
                                    show_message_cover("Nice shot!", 0);
                                    if (lv->monster[j]->hp[y2][x2] <= 0) {
                                        lv->monster[j]->hp[y2][x2] = 0;
                                        lv->monster[j]->cnt[y2][x2] = 0;
                                        show_message_cover("It's dead now.", 0);
                                        switch(lv->monster[j]->look) {
                                            case 'D' :mp->inv->gold_cnt += 5; break;
                                            case 'F' :mp->inv->gold_cnt += 7; break;
                                            case 'G' :mp->inv->gold_cnt += 15 + (10 * (player->difficulty / 2)); break;
                                            case 'S' :mp->inv->gold_cnt += 20 + (15 * (player->difficulty / 2)); break;
                                            case 'U' :mp->inv->gold_cnt += 30 + (15 * (player->difficulty / 2)); break;
                                            default : break;
                                        }
                                    }
                                    ct = 0;
                                    break;
                                }
                            }
                            if (ct == 0) break;
                            y = y2;
                            x = x2;
                        }
                        if (ct) {
                            if (lv->weapons->pickable_sit[y][x] == 1) {
                                if (strcmp(lv->weapons->cells[y][x], weapon_dagger)) {
                                    show_message_cover("Oops, your weapon broke!", 0);
                                } else {
                                    lv->weapons->cnt[y][x] += 1;
                                    show_message_cover("It fell.", 0);
                                }
                            } else {
                                strcpy(lv->weapons->cells[y][x], weapon_dagger);
                                lv->weapons->attr[y][x] = weapon_dagger_attr;
                                lv->weapons->cnt[y][x] = 1;
                                lv->weapons->pickable_sit[y][x] = 1;
                                show_message_cover("It fell.", 0);
                            }
                        }
                    }
                }
            }
            if (!strcmp(mp->inv->weapon_def, weapon_arrow) && mp->inv->weapon_def_attr == weapon_arrow_attr) {
                int dmg = 5;
                if (enchant_damage) dmg *= 2;
                int range = 5;
                if (!mp->inv->weapon_arrow_cnt) {
                    show_message_cover("Nothing left.", 0);
                } else {
                    show_message_cover("Which way?", 0);
                    int ch = getch();
                    int wy = -1;
                    switch (ch) {
                        case 'w' : wy = 0; break;
                        case 'e' : wy = 5; break;
                        case 'd' : wy = 1; break;
                        case 'c' : wy = 6; break;
                        case 's' : wy = 2; break;
                        case 'z' : wy = 7; break;
                        case 'a' : wy = 3; break;
                        case 'q' : wy = 4; break;
                        default :
                            show_message_cover("ha?", 0);
                            break;
                    }
                    if (wy != -1) {
                        mp->inv->weapon_arrow_cnt -= 1;
                        int y = mp->hero_place.y; int x = mp->hero_place.x;
                        int ct = 1;
                        for (int i = 1; i <= range; i++) {
                            int y2 = y + del_y[wy];
                            int x2 = x + del_x[wy];
                            if (lv->cell[y2][x2] == 0) break;
                            if (lv->cell[y2][x2] == el_window) {
                                break;
                            }
                            if (lv->cell[y2][x2] == el_door || lv->hidden_sit[y2][x2]) {
                                break;
                            }
                            if (is_wall(lv->cell[y2][x2])) {
                                break;
                            }
                            for (int j = lv->monster_num - 1; j >= 0; --j) {
                                if (lv->monster[j]->cnt[y2][x2]) {
                                    lv->monster[j]->hp[y2][x2] -= dmg;
                                    lv->monster[j]->token[y][x] = lv->monster[j]->max_token;
                                    ct = 0;
                                    show_message_cover("Nice shot!", 0);
                                    if (lv->monster[j]->hp[y2][x2] <= 0) {
                                        lv->monster[j]->hp[y2][x2] = 0;
                                        lv->monster[j]->cnt[y2][x2] = 0;
                                        show_message_cover("It's dead now.", 0);
                                        switch(lv->monster[j]->look) {
                                            case 'D' :mp->inv->gold_cnt += 5; break;
                                            case 'F' :mp->inv->gold_cnt += 7; break;
                                            case 'G' :mp->inv->gold_cnt += 15 + (10 * (player->difficulty / 2)); break;
                                            case 'S' :mp->inv->gold_cnt += 20 + (15 * (player->difficulty / 2)); break;
                                            case 'U' :mp->inv->gold_cnt += 30 + (15 * (player->difficulty / 2)); break;
                                            default : break;
                                        }
                                    }
                                    break;
                                }
                            }
                            if (ct == 0) break;
                            y = y2;
                            x = x2;
                        }
                        if (ct) {
                            if (lv->weapons->pickable_sit[y][x] == 1) {
                                if (strcmp(lv->weapons->cells[y][x], weapon_arrow)) {
                                    show_message_cover("Oops, your weapon broke!", 0);
                                } else {
                                    lv->weapons->cnt[y][x] += 1;
                                    show_message_cover("It fell.", 0);
                                }
                            } else {
                                strcpy(lv->weapons->cells[y][x], weapon_arrow);
                                lv->weapons->attr[y][x] = weapon_arrow_attr;
                                lv->weapons->cnt[y][x] = 1;
                                lv->weapons->pickable_sit[y][x] = 1;
                                show_message_cover("It fell.", 0);
                            }
                        }
                    }
                }
            }
            if (!strcmp(mp->inv->weapon_def, weapon_magic_wand) && mp->inv->weapon_def_attr == weapon_magic_wand_attr) {
                int dmg = 15;
                if (enchant_damage) dmg *= 2;
                int range = 10;
                if (!mp->inv->weapon_magic_wand_cnt) {
                    show_message_cover("Oooo, When you've finished Magical wands?", 0);
                } else {
                    show_message_cover("Which way?", 0);
                    int ch = getch();
                    int wy = -1;
                    switch (ch) {
                        case 'w' : wy = 0; break;
                        case 'e' : wy = 5; break;
                        case 'd' : wy = 1; break;
                        case 'c' : wy = 6; break;
                        case 's' : wy = 2; break;
                        case 'z' : wy = 7; break;
                        case 'a' : wy = 3; break;
                        case 'q' : wy = 4; break;
                        default :
                            show_message_cover("ha?", 0);
                            break;
                    }
                    if (wy != -1) {
                        mp->inv->weapon_magic_wand_cnt -= 1;
                        int y = mp->hero_place.y; int x = mp->hero_place.x;
                        int ct = 1;
                        for (int i = 1; i <= range; i++) {
                            int y2 = y + del_y[wy];
                            int x2 = x + del_x[wy];
                            if (lv->cell[y2][x2] == 0) break;
                            if (lv->cell[y2][x2] == el_window) {
                                break;
                            }
                            if (lv->cell[y2][x2] == el_door || lv->hidden_sit[y2][x2]) {
                                break;
                            }
                            if (is_wall(lv->cell[y2][x2])) {
                                break;
                            }
                            for (int j = lv->monster_num - 1; j >= 0; --j) {
                                if (lv->monster[j]->cnt[y2][x2]) {
                                    lv->monster[j]->hp[y2][x2] -= dmg;
                                    lv->monster[j]->token[y][x] = lv->monster[j]->max_token;
                                    ct = 0;
                                    show_message_cover("Nice shot!", 0);
                                    if (lv->monster[j]->hp[y2][x2] <= 0) {
                                        lv->monster[j]->hp[y2][x2] = 0;
                                        lv->monster[j]->cnt[y2][x2] = 0;
                                        show_message_cover("It's dead now.", 0);
                                        switch(lv->monster[j]->look) {
                                            case 'D' :mp->inv->gold_cnt += 5; break;
                                            case 'F' :mp->inv->gold_cnt += 7; break;
                                            case 'G' :mp->inv->gold_cnt += 15 + (10 * (player->difficulty / 2)); break;
                                            case 'S' :mp->inv->gold_cnt += 20 + (15 * (player->difficulty / 2)); break;
                                            case 'U' :mp->inv->gold_cnt += 30 + (15 * (player->difficulty / 2)); break;
                                            default : break;
                                        }
                                    }
                                    lv->monster[j]->token[y2][x2] = 0;
                                    lv->monster[j]->max_token = 0;
                                    
                                    break;
                                }
                            }
                            if (ct == 0) break;
                            y = y2;
                            x = x2;
                        }
                        if (ct) {
                            if (lv->weapons->pickable_sit[y][x] == 1) {
                                if (strcmp(lv->weapons->cells[y][x], weapon_magic_wand)) {
                                    show_message_cover("Oops, your weapon broke!", 0);
                                } else {
                                    lv->weapons->cnt[y][x] += 1;
                                    show_message_cover("It fell.", 0);
                                }
                            } else {
                                strcpy(lv->weapons->cells[y][x], weapon_magic_wand);
                                lv->weapons->attr[y][x] = weapon_magic_wand_attr;
                                lv->weapons->cnt[y][x] = 1;
                                lv->weapons->pickable_sit[y][x] = 1;
                                show_message_cover("It fell.", 0);
                            }
                        }
                    }
                }
            }
            did = 1;
        } else if (ch == 'M') { 
            M_mode_on ^= 1;
        } else {
            did = move_player(mp, ch, is_force_on, do_pick);
        }

        if (did) {
            mp->time += 1;
            if (stamina == Max_stamina) hp += Max_hp / 30;
            if (stamina == 0) hp -= Max_hp / 50;
            if (mp->time % stamina_circle == 0) stamina -= Max_stamina / 25;
            enchant_speed -= Max_enchant / 10;
            enchant_damage -= Max_enchant / 10;
            if (mp->lvls[mp->curr_lvl]->room_id[mp->hero_place.y][mp->hero_place.x] == mp->lvls[mp->curr_lvl]->enchant_room_id) {
                hp -= Max_hp / 50;
            }
            if (enchant_speed > 0) {
                if (mp->time % 2) now_its_enemy_turn(mp, player);
            } else now_its_enemy_turn(mp, player);
        }

        if (hp < 0) hp = 0;
        if (hp > Max_hp) hp = Max_hp;
        if (stamina < 0) stamina = 0;
        if (stamina > Max_stamina) stamina = Max_stamina;
        if (enchant_speed < 0) enchant_speed = 0;
        if (enchant_speed > Max_enchant) enchant_speed = Max_enchant;
        if (enchant_damage < 0) enchant_damage = 0;
        if (enchant_damage > Max_enchant) enchant_damage = Max_enchant;
        refresh_game(mp, player);
    }
}

int main() {
    setlocale(LC_ALL,"");
    initscr();
    curs_set(0);
    noecho();
    start_color();
    use_default_colors();
    set_colors();
    init_elmnts();
    
    keypad(stdscr, true);
    cbreak();
    refresh();
    init_elements_movement();
    user *player = raw_user();
    // player->difficulty = dif_easy;
    
    map *mp = generate_map(player);
    play_with_user(mp, player);
   
    refresh();
    
    // getch();
    
    endwin();

}
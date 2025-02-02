#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <time.h>
#include <locale.h>

#include "menu.h"
#include "map.h"

#ifndef movement_h_defined
#define movement_h_defined

extern int hp, Max_hp, hp_bar_len;
extern int stamina, Max_stamina, stamina_bar_len;
extern int enchant_speed, enchant_damage, Max_enchant, enchant_bar_len;
extern int M_mode_on;
extern WINDOW *bar_win;
extern WINDOW *mp_win;
extern WINDOW *info_win;
extern WINDOW *mg_win;


void init_windows();

void init_elements_movement();

void show_message_cover (char *s, int line_num);

void show_message_continue (char *s);

void show_message_cover_int (int n, int line_num);

void show_message_continue_int (int n);

void power_bar_show ();

void refresh_map(map *mp, user *player);

void get_name_of_item_and_cnt_by_shape_and_attr_in_inv (Inventory *inv,char *shape, chtype attr, int *cnt, char **name);

void refresh_info_box (map *mp, user *player);

void refresh_game (map *mp, user *player);

void add_this_to_inv (map *mp, char *shape, chtype attr, int cnt);

void pick_pos(map *mp, int y, int x);

int move_one (map *mp, elmnt dest, int do_pick);

int move_player(map *mp, int way_ch, int is_force_one, int do_pick) ;

void select_item(WINDOW *win, char *categ, map *mp, user *player);

void open_inventory (map *mp, user *player);

int adjance (int y1, int x1, int y2, int x2);

void move_enemy (map *mp, user *player, int mon_id);

void enemy_attack (map *mp, user *player, int mon_id);

void now_its_enemy_turn (map *mp, user *player);

int treasure_room_is_empty(map *mp);

void play_with_user (map *mp, user * player);

void start_a_new_game (user * player);

#endif
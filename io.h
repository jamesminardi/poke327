#ifndef POKE327_IO_H
#define POKE327_IO_H

#include <ncurses.h>
#include <panel.h>
#include "components.h"
#include "globals.h"
#include "colors.h"
#include "character.h"
#include "Pos.h"
#include "map.h"
#include "db_parse.h"

#define TERMINAL_Y 24
#define TERMINAL_X 80


typedef enum window_type {
	win_top,
	win_battle,
	win_map,
	win_bottom,
	win_trainers,
	win_opponent_summary,
	win_player_summary,
	win_starters,
	num_windows
} window_type_t;

void io_init();
void io_end();
int rand_level();
void io_get_starter();
void io_display_map();
void io_trainer_battle(Npc *c);
void io_player_turn();

#endif //POKE327_IO_H

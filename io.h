#ifndef POKE327_IO_H
#define POKE327_IO_H

#include <ncurses.h>
#include <panel.h>
#include "globals.h"
#include "world.h"
#include "components.h"

#define TERMINAL_Y 24
#define TERMINAL_X 80


typedef enum window_type {
	win_top,
	win_battle,
	win_map,
	win_bottom,
	win_trainers,
	num_windows
} window_type_t;

WINDOW *windows[num_windows];
PANEL *panels[num_windows];

void io_init();
void io_end();
void io_display();
void io_player_turn();


#endif //POKE327_IO_H

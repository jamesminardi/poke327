#include "io.h"

World* world;

static void io_initColor() {
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}


void io_init() {
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	io_initColor();
	// Init windows
	windows[win_map] = newwin(MAP_Y, TERMINAL_X, 1, 0);
	windows[win_top] = newwin(1, TERMINAL_X, 0, 0);
	windows[win_bottom] = newwin(2, TERMINAL_X, TERMINAL_Y - 2, 0);
	windows[win_battle] = newwin(MAP_Y-4, TERMINAL_X-4, 3, 2);
	windows[win_trainers] = newwin(MAP_Y-4, TERMINAL_X-20, 3, 10);
	box(windows[win_map], 0, 0);
	box(windows[win_bottom], 0, 0);
	box(windows[win_battle],0,0);
	box(windows[win_trainers],0,0);

	// Attach a panel to each window		Order is bottom up
	panels[win_bottom] = new_panel(windows[win_bottom]);
	panels[win_map] = new_panel(windows[win_map]);
	panels[win_top] = new_panel(windows[win_top]);
	panels[win_battle] = new_panel(windows[win_battle]);
	panels[win_trainers] = new_panel(windows[win_trainers]);

	hide_panel(panels[win_battle]);
	hide_panel(panels[win_trainers]);

	update_panels(); // Write panels to vitual screen in correct visibility order
	doupdate(); // Shows to screen
}

void io_end(){
	endwin();
//	while(io_head) {
//		io_tail = io_head;
//		io_head = io_head->next;
//		free(io_tail);
//	}
//	io_tail = NULL;
}

void io_display() {
	wclear(windows[win_map]);
	int x, y;
	for (y = 0; y < MAP_Y; y++) {
		for(x = 0; x < MAP_X; x++) {
			if (world->getMap(x,y)->getChar(x,y)) {
				mvwaddch(windows[win_map], y, x, char_getSymbol(world->getMap(x,y)->getChar(x,y)->type));
			} else {
				switch (world->getMap(x,y)->getTerrain(x,y)) {
					case ter_boulder:
					case ter_mountain:
						attron(COLOR_PAIR(COLOR_MAGENTA));
						mmvwaddch(windows[win_map],y, x, char_getSymbol(ter_boulder));
						attroff(COLOR_PAIR(COLOR_MAGENTA));
						break;
					case ter_tree:
					case ter_forest:
						attron(COLOR_PAIR(COLOR_GREEN));
						mvwaddch(windows[win_map],y, x, char_getSymbol(ter_tree));
						attroff(COLOR_PAIR(COLOR_GREEN));
						break;
					case ter_path:
					case ter_exit:
						attron(COLOR_PAIR(COLOR_YELLOW));
						mvwaddch(windows[win_map],y, x, char_getSymbol(ter_path));
						attroff(COLOR_PAIR(COLOR_YELLOW));
						break;
					case ter_mart:
						attron(COLOR_PAIR(COLOR_BLUE));
						mvwaddch(windows[win_map],y, x, char_getSymbol(ter_mart));
						attroff(COLOR_PAIR(COLOR_BLUE));
						break;
					case ter_center:
						attron(COLOR_PAIR(COLOR_RED));
						mvwaddch(windows[win_map],y, x, char_getSymbol(ter_center));
						attroff(COLOR_PAIR(COLOR_RED));
						break;
					case ter_grass:
						attron(COLOR_PAIR(COLOR_GREEN));
						mvwaddch(windows[win_map],y, x, char_getSymbol(ter_grass));
						attroff(COLOR_PAIR(COLOR_GREEN));
						break;
					case ter_clearing:
						attron(COLOR_PAIR(COLOR_GREEN));
						mvwaddch(windows[win_map],y, x, char_getSymbol(ter_clearing));
						attroff(COLOR_PAIR(COLOR_GREEN));
						break;
					default:
						attron(COLOR_PAIR(COLOR_CYAN));
						mvwaddch(windows[win_map],y, x, '0');
						attroff(COLOR_PAIR(COLOR_CYAN));
				}
			}
		}
	}
	update_panels();
	doupdate();
}

void io_player_turn() {
	int key;
	int turn_consumed;
	int moving;
	Pos newPos;
	static int random_encounter_buffer_count = RANDOM_ENCOUNTER_BUFFER;
	// Flush input buffer to avoid past inputs from getting used
	turn_consumed = 0;
	moving = 0;
	newPos = c->pos;
	while (!turn_consumed) {
		world_updateScreen();
		io_display();
		flushinp();
		key = getch();
		wclear(windows[win_top]);
		update_panels();
		doupdate();
		switch (key) {
			// NorthWest
			case '7':   // 7
			case 'y':   // y
				newPos = (Pos) {c->pos.x + all_dirs[dir_northwest].x, c->pos.y + all_dirs[dir_northwest].y};
				moving = 1;
				break;
				// North
			case '8':    // 8
			case 'k':    // k
				newPos = (Pos) {c->pos.x + all_dirs[dir_north].x, c->pos.y + all_dirs[dir_north].y};
				moving = 1;
				break;
				// NorthEast
			case '9':    // 9
			case 'u':    // u
				newPos = (Pos) {c->pos.x + all_dirs[dir_northeast].x, c->pos.y + all_dirs[dir_northeast].y};
				moving = 1;
				break;
				// West
			case '4':    // 4
			case 'h':    // h
				newPos = (Pos) {c->pos.x + all_dirs[dir_west].x, c->pos.y + all_dirs[dir_west].y};
				moving = 1;
				break;
				// East
			case '6':    // 6
			case 'l':    // l
				newPos = (Pos) {c->pos.x + all_dirs[dir_east].x, c->pos.y + all_dirs[dir_east].y};
				moving = 1;
				break;
				// SouthWest
			case '1':    // 1
			case 'b':    // b
				newPos = (Pos) {c->pos.x + all_dirs[dir_southwest].x, c->pos.y + all_dirs[dir_southwest].y};
				moving = 1;
				break;
				// South
			case '2':    // 2
			case 'j':    // j
				newPos = (Pos) {c->pos.x + all_dirs[dir_south].x, c->pos.y + all_dirs[dir_south].y};
				moving = 1;
				break;
				// SouthEast
			case '3':    // 3
			case 'n':    // n
				newPos = (Pos) {c->pos.x + all_dirs[dir_southeast].x, c->pos.y + all_dirs[dir_southeast].y};
				moving = 1;
				break;

				// NOP
			case '5':    // 5
			case ' ':    // space
			case '.':    // .
				turn_consumed = 1;
				moving = 0;
				break;

				// Enter a building
			case '>':    // >
				wclear(windows[win_top]);
				mvwaddstr(windows[win_top], 0, 0, "Entering a building not yet implemented.");
				break;

				// List of trainers
			case 't':    // t
				listTrainers();
				moving = 0;
				turn_consumed = 0;
				break;

				// Quit the game
			case 'q':    // q
			case 'Q':	 // Q
				turn_consumed = 1;
				world.quit = 1;
				break;
				// Scroll up list
			case KEY_UP: // up arrow
				// Scroll down list
			case KEY_DOWN: // down arrow
				// Return from list
			case 27:    // esc
			default:
				wclear(windows[win_top]);
				mvwaddstr(windows[win_top], 0, 0, "Invalid action.");
		} // switch(key)
		if (world.quit) {
			return;
		}

		// Check if PC can move there & Check if NPC exists and is defeated
		if (moving && move_cost[c->type][mappos(newPos)] != INT_MAX && charpos(newPos) && ((Npc*)charpos(newPos))->defeated) {
			wclear(windows[win_top]);
			mvwaddstr(windows[win_top], 0, 0, "That trainer has already been defeated.");
		}
		// Check if PC can move there & Check if NPC exists  amd is undefeated
		// Battle npc
		if (moving && move_cost[c->type][mappos(newPos)] != INT_MAX && charpos(newPos) && !((Npc*)charpos(newPos))->defeated && charpos(newPos) != charpos(world.pc->pos)) {
			// Battle character
			npc_battle((Npc *) charpos(newPos));
			// Does not use up turn, so player can move again after defeating and NPC
		}
		// Check if PC can move there & Check if unoccupied by NPC
		// Move to that space
		if (moving && move_cost[c->type][mappos(newPos)] != INT_MAX && mappos(newPos) != ter_exit && !charpos(newPos)) {
			// Move to that location
			move_char(c, newPos);
			// Check for random encounter (10%)
			if (mappos(newPos) == ter_grass && random_encounter_buffer_count < 1 && rand() % 10 == 0) {
				random_battle();
				random_encounter_buffer_count = RANDOM_ENCOUNTER_BUFFER;
			}
			random_encounter_buffer_count--;
			turn_consumed = 1;
		}

		if (moving && mappos(newPos) == ter_exit) {
			// Change maps
			if (newPos.y < 1) { 		// N
				world_changeMap((Pos){world.cur_idx.x, world.cur_idx.y - 1}, world.cur_idx);
			}
			if (newPos.y >= MAP_Y - 1) { // S
				world_changeMap((Pos){world.cur_idx.x, world.cur_idx.y + 1}, world.cur_idx);
			}
			if (newPos.x >= MAP_X - 1) { // E
				world_changeMap((Pos){world.cur_idx.x + 1, world.cur_idx.y}, world.cur_idx);
			}
			if (newPos.x < 1) {        // W
				world_changeMap((Pos) {world.cur_idx.x - 1, world.cur_idx.y}, world.cur_idx);
			}
			if (((Character *)heap_peek_min(&world.cur_map->turn)) != NULL) {
				world.pc->next_turn = ((Character *) heap_peek_min(&world.cur_map->turn))->next_turn;
				turn_consumed = 1;
			}
		}

	} // while (!done)

	// calculate next turn and reinsert into
	// If the PC moved to a new map, it's next turn is the minimum in the heap + the tile cost
	c->next_turn += move_cost[char_pc][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
#include "world.h"

// /*
//  * Constructor
//  */
// World::World(int seed) {
//	 srand(seed);
//	 // Initialize world to maps of null
//	 this->quit_game_flag = 0;
//
//	 // Set spawn map location
//	 this->cur_idx = new Pos((WORLD_X - 1 ) / 2, (WORLD_Y - 1) / 2);
//
//	 this->pc = new Pc();
//
//		 int x;
//		 int y;
//		 for (x = 0; x < WORLD_X; x++) {
//			 for (y = 0; y < WORLD_Y; y++) {
//				 worldxy(x, y) = NULL;
//			 }
//		 }
//		 // Initialize spawn map
//		 //world_pcInit();
//		 world_changeMap(world.cur_idx, world.cur_idx);
//}
//
//World::~World() {
//	 delete this->cur_idx;
//	 delete this->pc;
// }


/*
 * Initializes the map struct and members
 */
static void world_initMap() {
	int x,y;
	worldxy(world.cur_idx.x , world.cur_idx.y) = (map_t*)malloc(sizeof(*world.cur_map));
	world.cur_map = worldxy(world.cur_idx.x, world.cur_idx.y);

	/* Set exit positions */
	if (world.cur_idx.y - 1 > 0 && worldxy(world.cur_idx.x, world.cur_idx.y - 1) != NULL) {
		world.cur_map->north = worldxy(world.cur_idx.x, world.cur_idx.y - 1)->south;
	} else {
		world.cur_map->north = rand() % (MAP_X - 4 ) + 2;
	}
	if (world.cur_idx.y + 1 < WORLD_Y && worldxy(world.cur_idx.x, world.cur_idx.y + 1) != NULL) {
		world.cur_map->south = worldxy(world.cur_idx.x, world.cur_idx.y + 1)->north;
	} else {
		world.cur_map->south = rand() % (MAP_X - 4) + 2;
	}
	if (world.cur_idx.x + 1 < WORLD_X && worldxy(world.cur_idx.x + 1, world.cur_idx.y) != NULL) {
		world.cur_map->east = worldxy(world.cur_idx.x + 1, world.cur_idx.y)->west;
	} else {
		world.cur_map->east = rand() % (MAP_Y - 4) + 2;
	}
	if (world.cur_idx.x - 1 > 0 && worldxy(world.cur_idx.x - 1, world.cur_idx.y) != NULL) {
		world.cur_map->west = worldxy(world.cur_idx.x - 1, world.cur_idx.y)->east;
	} else {
		world.cur_map->west = rand() % (MAP_Y - 4) + 2;
	}

	/* Remove road exits on edge of world */
	if (world.cur_idx.x == 0) {
		world.cur_map->m[world.cur_map->west][0] = ter_border;
	}
	if (world.cur_idx.x == WORLD_X - 1) {
		world.cur_map->m[world.cur_map->east][MAP_X - 1] = ter_border;
	}
	if (world.cur_idx.y == 0) {
		world.cur_map->m[0][world.cur_map->north] = ter_border;
	}
	if (world.cur_idx.y == WORLD_Y - 1) {
		world.cur_map->m[MAP_Y - 1][world.cur_map->south] = ter_border;
	}

	// Init character map to all unoccupied and tile map to clearing
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			world.cur_map->m[y][x] = ter_clearing;
			world.cur_map->char_m[y][x] = NULL;
		}
	}
	heap_init(&world.cur_map->turn, turn_cmp, heap_delete_char);
}


/**
 * @param to 	Index of destination map in world units
 * @param from 	Index of departure map in world units
 */
void world_changeMap(pos_t to, pos_t from) {

	// Index of destination map relative to departure map
	pos_t relative_toDir = (pos_t){to.x - from.x, to.y - from.y};

	if (to.x < 0 || to.x > WORLD_X - 1 || to.y < 0 || to.y > WORLD_Y - 1) {
		// ERROR: Out of bounds
		//printf("Out of bounds. Position did not change\n");
		return;
	}

	// Update current map index
	world.cur_idx = to;
	// Remove pc from previous map
	if (world.pc) {
		charpos(world.pc->pos) = NULL;
	}

	// Generate new map
	if (worldxy(world.cur_idx.x, world.cur_idx.y) == NULL) {
		world_initMap(); // Update cur_map
		terrain_init(world.cur_map);
		pc_init(relative_toDir);
		// Put a pointer to the PC into the destination map
		//charpos(world.pc->pos) = world.pc;
		//world.pc->next_turn = 0;
		npc_init();

	} else {
		// If destination map does exist, set current map to that map
		world.cur_map = worldxy(world.cur_idx.x, world.cur_idx.y);
		pc_init(relative_toDir);
	}


}

void world_init() {
	srand(world.seed);

	// Set spawn map location
	world.cur_idx.x = (WORLD_X -1) / 2;
	world.cur_idx.y = (WORLD_Y- 1) / 2;
	world.quit_game_flag = 0;
	// Initialize world to maps of null
	int x;
	int y;
	for (x = 0; x < WORLD_X; x++) {
		for (y = 0; y < WORLD_Y; y++) {
			worldxy(x, y) = NULL;
		}
	}
	// Initialize spawn map
	//world_pcInit();
	world_changeMap(world.cur_idx, world.cur_idx);
}

/**
 * TODO delete all characters as well in all maps
 */
void world_delete() {

	int x;
	int y;
	for (x = 0; x < WORLD_X; x++) {
		for (y = 0; y < WORLD_Y; y++) {
			if (worldxy(x, y) != NULL) {
				free(worldxy(x, y));
				worldxy(x, y) = NULL;
			}
		}
	}

}

static void neighbor_init(pos_t *neighbors, path_t *center) {
	neighbors[0].y = center->pos.y - 1;
	neighbors[0].x = center->pos.x - 1;

	neighbors[1].y = center->pos.y - 1;
	neighbors[1].x = center->pos.x;

	neighbors[2].y = center->pos.y - 1;
	neighbors[2].x = center->pos.x + 1;

	neighbors[3].y = center->pos.y;
	neighbors[3].x = center->pos.x - 1;

	neighbors[4].y = center->pos.y;
	neighbors[4].x = center->pos.x + 1;

	neighbors[5].y = center->pos.y + 1;
	neighbors[5].x = center->pos.x - 1;

	neighbors[6].y = center->pos.y + 1;
	neighbors[6].x = center->pos.x;

	neighbors[7].y = center->pos.y + 1;
	neighbors[7].x = center->pos.x + 1;
}

static void turn_neighbor_init(pos_t *neighbors, Character *center) {
	neighbors[dir_north].y = center->pos.y - 1;
	neighbors[dir_north].x = center->pos.x;

	neighbors[dir_south].y = center->pos.y + 1;
	neighbors[dir_south].x = center->pos.x;

	neighbors[dir_east].y = center->pos.y;
	neighbors[dir_east].x = center->pos.x + 1;

	neighbors[dir_west].y = center->pos.y;
	neighbors[dir_west].x = center->pos.x - 1;

	neighbors[dir_northeast].y = center->pos.y - 1;
	neighbors[dir_northeast].x = center->pos.x + 1;

	neighbors[dir_northwest].y = center->pos.y - 1;
	neighbors[dir_northwest].x = center->pos.x - 1;

	neighbors[dir_southeast].y = center->pos.y + 1;
	neighbors[dir_southeast].x = center->pos.x + 1;

	neighbors[dir_southwest].y = center->pos.y + 1;
	neighbors[dir_southwest].x = center->pos.x - 1;

}


/*
 * Find manhattan distance from x,y
 */
static int find_manhattan_distance(pos_t p) {
	return abs(p.x - ((WORLD_X - 1) / 2)) + abs(p.y - ((WORLD_Y - 1) / 2));
}

void random_battle() {
	int done;
	int key;
	int pid = rand() % 1092 + 1;
	int shiny;
	char shinystr[20];
	int genderid;
	char genderstr[20];
	move_db move1;
	move_db move2;
	char stats[400];
	char hp[200];
	char atk[200];
	char def[200];
	char spatk[200];
	char spdef[200];
	char speed[200];
	if (rand() % 8192 == 0) {
		shiny = 1;
	} else {
		shiny = 0;
	}
	genderid = rand() % 2;

	if (genderid) {
		sprintf(genderstr, "gender: male");
	} else {
		sprintf(genderstr, "gender: female");
	}

	if (shiny) {
		sprintf(shinystr, "shiny: true");
	} else {
		sprintf(shinystr, "shiny: false");
	}


	// Calculate level
	int min_level, max_level, level;
	int dist = abs(world.cur_idx.x - ((WORLD_X - 1) / 2)) + abs(world.cur_idx.y - ((WORLD_Y - 1) / 2));
	if (dist <= 200) {
		min_level = 1;
		max_level = dist / 2;
	} else {
		min_level = (dist - 200) / 2;
		max_level = 100;
	}
	if (max_level < 1) {
		max_level = 1;
	}
	level = rand() % (max_level - min_level + 1) + min_level;


	// Calculate moves (After doing level)
	std::vector<move_db> valid_moves;
	int i;
	for (pokemon_move_db m : pokemon_moves) {
		if (m.pokemon_id == pokemon[pid].species_id && m.version_group_id == 19 &&
			m.pokemon_move_method_id == 1 && level >= m.level) {
			for (move_db fmove : moves) {
				if (fmove.id == m.move_id) {
					valid_moves.push_back(fmove);
					break;
				}
			}
		}
	}

	int move1_done = 0;
	int move2_done = 0;
	while (!valid_moves.empty()) {
		if (!move1_done) {
			int rand_move = rand() % valid_moves.size();
			move1 = valid_moves.at(rand_move);
			valid_moves.erase(valid_moves.begin() + rand_move);
			move1_done = 1;
		}
		else {
			int rand_move = rand() % valid_moves.size();
			if (move1.id == valid_moves.at(rand_move).id) {
				valid_moves.erase(valid_moves.begin() + rand_move);
			} else {
				move2 = valid_moves.at(rand_move);
				move2_done = 1;
				break;
			}
		}
	}
	if (!move1_done) {
		move_db tmp;
		tmp.id = -1;
		sprintf(tmp.identifier, "No available move in v19");
		move1 = tmp;
	}
	if (!move2_done) {
		move_db tmp;
		tmp.id = -1;
		sprintf(tmp.identifier, "No available move in v19");
		move2 = tmp;
	}


//	if (valid_moves.size() > 1) {
//		int move1_id = rand() % valid_moves.size();
//		for (move_db m1 : valid_moves) {
//			if (m1.id == (valid_moves.begin() + move1_id)->id) {
//				move1 = m1;
//			}
//		}
//		int move2_id = rand() % valid_moves.size();
//		for (move_db m2 : valid_moves) {
//			if (m2.id == (valid_moves.begin() + move2_id)->id && m2.id != move1.id) {
//				move2 = m2;
//			}
//		}
//	}
//	if (valid_moves.size() == 1) {
//		int move2_id = rand() % valid_moves.size();
//		for (auto && m : valid_moves) {
//			if (m.id == move2_id && m.id != move1.id) {
//				move1 = m;
//			}
//		}
//		move_db mov;
//		sprintf(mov.identifier, "No available move in v19");
//		move2 = mov;
//	}
//	if (valid_moves.size() == 0) {
//		move_db mov;
//		sprintf(mov.identifier, "No available move in v19");
//		move1 = mov;
//		move2 = mov;
//	}


	//calculate stats
	for (auto &&p : pokemon_stats) {
		if (p.pokemon_id == pokemon[pid].id) {
			switch (p.stat_id) {
				case 1: // hp
					std::sprintf(hp, "hp: %d\tiv: %d", p.base_stat, rand() % 16);
					break;
				case 2: //atk
					std::sprintf(atk, "atk: %d\tiv: %d", p.base_stat, rand() % 16);
					break;
				case 3: //def
					std::sprintf(def, "def: %d\tiv: %d", p.base_stat, rand() % 16);
					break;
				case 4: // spa
					std::sprintf(spatk, "SPatk: %d\tiv: %d", p.base_stat, rand() % 16);
					break;
				case 5: //spd
					std::sprintf(spdef, "SPdef: %d\tiv: %d", p.base_stat, rand() % 16);
					break;
				case 6: //speed
					std::sprintf(speed, "speed: %d\tiv: %d", p.base_stat, rand() % 16);
					break;
				default:
					break;
			}
		}
	}

	std::sprintf(stats,
			"A wild %s appeard!\n"
				  " id: %d\n"
				  " specied_id: %d\n"
				  " height: %d\n"
				  " weight: %d\n"
				  " order: %d\n"
				  " is_default: %d\n"
				  " level: %d\n"
				  " move1: %s\n"
				  " move2: %s\n\n"
				  " PRESS ESC TO DEFEAT...",
				 pokemon[pid].identifier,
				 pokemon[pid].id,
				 pokemon[pid].species_id,
				 pokemon[pid].height,
				 pokemon[pid].weight,
				 pokemon[pid].order,
				 pokemon[pid].is_default,
				 level,
				 move1.identifier,
				 move2.identifier);


	show_panel(panels[win_battle]);
	wclear(windows[win_battle]);

	done = 0;
	while (!done) {

		mvwaddstr(windows[win_battle], 1,1, stats);
		mvwaddstr(windows[win_battle], 1,45, hp);
		mvwaddstr(windows[win_battle], 2,45, atk);
		mvwaddstr(windows[win_battle], 3,45, def);
		mvwaddstr(windows[win_battle], 4,45, spatk);
		mvwaddstr(windows[win_battle], 5,45, spdef);
		mvwaddstr(windows[win_battle], 6,45, speed);
		mvwaddstr(windows[win_battle], 7,45, genderstr);
		mvwaddstr(windows[win_battle], 8,45, shinystr);
		box(windows[win_battle],0,0);
		update_panels();
		doupdate();

		key = getch();
		switch (key) {
			case 27: // ESC
				wclear(windows[win_battle]);
				hide_panel(panels[win_battle]);
				// PC defeats pokemon
				// Return to map
				done = 1;
				break;
		}
	}
}


void world_updateScreen() {
	wclear(windows[win_map]);
	int x, y;
	for (y = 0; y < MAP_Y; y++) {
		for(x = 0; x < MAP_X; x++) {
			if (world.cur_map->char_m[y][x]) {
				mvwaddch(windows[win_map], y, x, char_getSymbol(world.cur_map->char_m[y][x]->type));
			} else {
				mvwaddch(windows[win_map], y, x, ter_getSymbol(world.cur_map->m[y][x]));
			}
		}
	}
	update_panels();
	doupdate();
	//wrefresh();
}

static std::string relativePosString(int x, int y) {
	pos_t relPos;
	relPos.x = x - world.pc->pos.x;
	relPos.y = y - world.pc->pos.y;
	std::string str = "(" + std::to_string(relPos.x) + ", " + std::to_string(relPos.y) + ")";
	return str;

}

void listTrainers() {
	// Battle
	int done;
	int key;
	done  = 0;
	// Right now, display to panel
	show_panel(panels[win_trainers]);
	top_panel(panels[win_trainers]);
	wclear(windows[win_trainers]);

	box(windows[win_trainers],0,0);
	mvwaddstr(windows[win_trainers], 1,1,"Trainers:");
	int x, y;
	int row = 1;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (world.cur_map->char_m[y][x]) {
				mvwprintw(windows[win_trainers], row, 1, "%s at %s relative",
						  char_getString(world.cur_map->char_m[y][x]->type).c_str(), relativePosString(x,y).c_str());
				row++;
			}
		}
	}
	while (!done) {

		update_panels();
		doupdate();
		key = getch();
		switch (key) {
			case 27: // ESC
				hide_panel(panels[win_trainers]);
				done = 1;
				break;
		}
	}
}

static void char_pcTurn(Character *c) {
	int key;
	int done;
	int moving;
	pos_t newPos;
	static int random_encounter_buffer_count = RANDOM_ENCOUNTER_BUFFER;
	// Flush input buffer to avoid past inputs from getting used
	done = 0;
	moving = 0;
	newPos = c->pos;
	while (!done) {
		world_updateScreen();
		flushinp();
		key = getch();
		wclear(windows[win_top]);
		update_panels();
		doupdate();
		switch (key) {
			// NorthWest
			case '7':   // 7
			case 'y':   // y
				newPos = (pos_t) {c->pos.x + all_dirs[dir_northwest].x, c->pos.y + all_dirs[dir_northwest].y};
				moving = 1;
				break;
				// North
			case '8':    // 8
			case 'k':    // k
				newPos = (pos_t) {c->pos.x + all_dirs[dir_north].x, c->pos.y + all_dirs[dir_north].y};
				moving = 1;
				break;
				// NorthEast
			case '9':    // 9
			case 'u':    // u
				newPos = (pos_t) {c->pos.x + all_dirs[dir_northeast].x, c->pos.y + all_dirs[dir_northeast].y};
				moving = 1;
				break;
				// West
			case '4':    // 4
			case 'h':    // h
				newPos = (pos_t) {c->pos.x + all_dirs[dir_west].x, c->pos.y + all_dirs[dir_west].y};
				moving = 1;
				break;
				// East
			case '6':    // 6
			case 'l':    // l
				newPos = (pos_t) {c->pos.x + all_dirs[dir_east].x, c->pos.y + all_dirs[dir_east].y};
				moving = 1;
				break;
				// SouthWest
			case '1':    // 1
			case 'b':    // b
				newPos = (pos_t) {c->pos.x + all_dirs[dir_southwest].x, c->pos.y + all_dirs[dir_southwest].y};
				moving = 1;
				break;
				// South
			case '2':    // 2
			case 'j':    // j
				newPos = (pos_t) {c->pos.x + all_dirs[dir_south].x, c->pos.y + all_dirs[dir_south].y};
				moving = 1;
				break;
				// SouthEast
			case '3':    // 3
			case 'n':    // n
				newPos = (pos_t) {c->pos.x + all_dirs[dir_southeast].x, c->pos.y + all_dirs[dir_southeast].y};
				moving = 1;
				break;

				// NOP
			case '5':    // 5
			case ' ':    // space
			case '.':    // .
				done = 1;
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
				done = 0;
				break;

				// Quit the game
			case 'q':    // q
			case 'Q':	 // Q
				done = 1;
				world.quit_game_flag = 1;
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
		if (world.quit_game_flag) {
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
			done = 1;
		}

		if (moving && mappos(newPos) == ter_exit) {
			// Change maps
			if (newPos.y < 1) { 		// N
				world_changeMap((pos_t){world.cur_idx.x, world.cur_idx.y - 1}, world.cur_idx);
			}
			if (newPos.y >= MAP_Y - 1) { // S
				world_changeMap((pos_t){world.cur_idx.x, world.cur_idx.y + 1}, world.cur_idx);
			}
			if (newPos.x >= MAP_X - 1) { // E
				world_changeMap((pos_t){world.cur_idx.x + 1, world.cur_idx.y}, world.cur_idx);
			}
			if (newPos.x < 1) {        // W
				world_changeMap((pos_t) {world.cur_idx.x - 1, world.cur_idx.y}, world.cur_idx);
			}
			if (((Character *)heap_peek_min(&world.cur_map->turn)) != NULL) {
				world.pc->next_turn = ((Character *) heap_peek_min(&world.cur_map->turn))->next_turn;
				done = 1;
			}
		}

	} // while (!done)

	// calculate next turn and reinsert into
	// If the PC moved to a new map, it's next turn is the minimum in the heap + the tile cost
	c->next_turn += move_cost[char_pc][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}

void world_gameLoop() {
	Character *curr_char;
	pathfind(world.cur_map, world.rival_dist, char_rival, world.pc->pos);
	pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);

	world_updateScreen();
	while (!world.quit_game_flag) {
	// old while: heap_peek_min(&world.cur_map->turn)
		curr_char = ((Character *) heap_remove_min(&world.cur_map->turn));
		switch (curr_char->type) {

			case char_pc:
				world_updateScreen();
				char_pcTurn(curr_char);
				if (move_cost[char_hiker][mappos(world.pc->pos)] != INT_MAX) {
					pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);
				}
				if (move_cost[char_rival][mappos(world.pc->pos)] != INT_MAX) {
					pathfind(world.cur_map, world.rival_dist, char_hiker, world.pc->pos);
				}
				break;
			case char_rival:
				char_rivalTurn((Npc*)curr_char);
				break;
			case char_hiker:
				char_hikerTurn((Npc*)curr_char);
				break;
			case char_statue:
				char_statueTurn((Npc*)curr_char);
				break;
			case char_pacer:
				char_pacerTurn((Npc*)curr_char);
				break;
			case char_wanderer:
				char_wandererTurn((Npc*)curr_char);
				break;
			case char_random:
				char_randomTurn((Npc*)curr_char);
				break;
			default:
				break;
		} // switch curr_char
	} // while(!quit_game)


}
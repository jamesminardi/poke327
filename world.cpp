#include "world.h"

 /*
  * Constructor
  */
World::World(int seed, int num_trainers) : seed(seed), num_trainers(num_trainers){
	srand(this->seed);
	// Initialize world to maps of null
	quit = 0;

	// Set spawn map location
	cur_idx = Pos((WORLD_X - 1 ) / 2, (WORLD_Y - 1) / 2);
	cur_map = w[cur_idx.y][cur_idx.x];
	pc = new Pc();
	int x;
	int y;
	for (x = 0; x < WORLD_X; x++) {
		for (y = 0; y < WORLD_Y; y++) {
			w[y][x] = nullptr;
		}
	}
	cur_map = new Map();
	pc->pos = rand_path(cur_map);
	cur_map->placeCharacters(pc);
	// Initialize spawn map
	//world_pcInit();
	this->changeMap(cur_idx);
}

World::~World() {
	this->cur_idx;
	delete this->pc;

	int x;
	int y;
	for (x = 0; x < WORLD_X; x++) {
		for (y = 0; y < WORLD_Y; y++) {
			if (w[y][x]) {
				delete w[y][x];
			}
		}
	}

}
Map* World::getMap(int x, int y){
	return this->w[y][x];
}
Map* World::getMap(Pos pos) {
	return this->w[pos.y][pos.x];
}
void World::setMap(int x, int y, Map* map){
	this->w[y][x] = map;
}
void World::setMap(Pos pos, Map* map) {
	this->w[pos.y][pos.x] = map;
}

/*
 * Main game loop
 */
void World::gameLoop() {
	Character *curr_char;
	pathfind(cur_map, rival_dist, char_rival, pc->pos);
	pathfind(cur_map, hiker_dist, char_hiker, pc->pos);

	io_display();
	while (!quit) {
		// old while: heap_peek_min(&world.cur_map->turn)
		curr_char = ((Character *) heap_remove_min(&world.cur_map->turn));
		switch (curr_char->type) {

			case char_pc:
				io_display();

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

/*
 * cur_idx refers to previous map, to refers to new map index
 * cur_map refers to previous map
 * If map at pos(to) has already been generated, then all this
 * does is change cur_idx and cur_map
 * Otherwise, generate the map
 */
void World::changeMap(Pos to){

	// Index of destination map relative to departure map
	Pos relative_toDir = Pos(to.x - cur_idx.x, to.y - cur_idx.y);

	if (to.x < 0 || to.x > WORLD_X - 1 || to.y < 0 || to.y > WORLD_Y - 1) {
		// ERROR: Out of bounds
		//printf("Out of bounds. Position did not change\n");
		return;
	}

	// Update current map index
	cur_idx = to;
	// Remove pc from previous map
	if (pc) {
		cur_map->setChar(pc->pos, nullptr);
	}

	if (this->getMap(cur_idx)) {
		// If destination map does exist, set current map to that map
		cur_map = this->getMap(cur_idx);
		cur_map->placePc(pc, relative_toDir);

	} else {
		// Generate new map
		this->initMap(); // Update cur_map
		cur_map->placePc(pc, relative_toDir);
		cur_map->placeNpc();
	}
}

/*
 * Initializes map at cur_idx, setting exits
 * Assumes 	cur_idx is set to map to initialize
 * 			cur_map is not set
 */
void World::initMap(){
	int x,y;
	int north,south,east,west;

	/* Set exit positions */
	if (cur_idx.y - 1 > 0 && this->getMap(cur_idx.x, cur_idx.y - 1) != nullptr) {
		north = this->getMap(cur_idx.x, cur_idx.y - 1)->south;
	} else {
		north = rand() % (MAP_X - 4 ) + 2;
	}
	if (cur_idx.y + 1 < WORLD_Y && this->getMap(cur_idx.x, cur_idx.y + 1) != NULL) {
		south = this->getMap(cur_idx.x, cur_idx.y + 1)->north;
	} else {
		south = rand() % (MAP_X - 4) + 2;
	}
	if (cur_idx.x + 1 < WORLD_X && this->getMap(cur_idx.x + 1, cur_idx.y) != NULL) {
		east = this->getMap(cur_idx.x + 1, cur_idx.y)->west;
	} else {
		east = rand() % (MAP_Y - 4) + 2;
	}
	if (cur_idx.x - 1 > 0 && this->getMap(cur_idx.x - 1, cur_idx.y) != NULL) {
		west = this->getMap(cur_idx.x - 1, cur_idx.y)->east;
	} else {
		west = rand() % (MAP_Y - 4) + 2;
	}

	// Init map
	this->setMap(cur_idx, new Map(north,south,east,west));
	cur_map = getMap(cur_idx);

	/* Remove road exits on edge of world */
	if (cur_idx.x == 0) {
		cur_map->setTerrain(cur_map->west, 0, ter_border);
	}
	if (cur_idx.x == WORLD_X - 1) {
		cur_map->setTerrain(cur_map->east, MAP_X - 1, ter_border);
	}
	if (cur_idx.y == 0) {
		cur_map->setTerrain(0, cur_map->north, ter_border);
	}
	if (cur_idx.y == WORLD_Y - 1) {
		cur_map->setTerrain(MAP_Y - 1, cur_map->south, ter_border);
	}
}

void move_char(Character *c, Pos pos) {
	// if moving the PC to itself, then check if the PC exists in the charmap somewhere else other than it's position
	// Then move that PC position to its current position
	if (c->type == char_pc && !charpos(c->pos) && (pos.x == c->pos.x && pos.y == c->pos.y)) {
		charpos(pos) = world.pc;
		world.pc->pos = pos;
	}

	if (c->type == char_pc && !charpos(c->pos) && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		charpos(pos) = world.pc;
		world.pc->pos = pos;
	}
	if (c->type == char_pc && charpos(c->pos) && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		charpos(pos) = world.pc;
		charpos(c->pos) = NULL;
		world.pc->pos = pos;
	}

	// Set character position in character map
	if (c->type != char_pc && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		charpos(pos) = charpos(c->pos);
		charpos(c->pos) = NULL;
		// Set character inner position
		charpos(pos)->pos = pos;
	}
}

/*
 * Assumes the PC was removed from the map if previously visisted
 *
 */
void pc_init(Pos relative_toDir) {

	// Set pc position to correct exit location based on the previous map
	if (relative_toDir.x == all_dirs[dir_north].x && relative_toDir.y == all_dirs[dir_north].y) {
		world.pc->pos.x = world.cur_map->south;
		world.pc->pos.y = MAP_Y - 2;
		move_char(world.pc, world.pc->pos);
	}
	else if (relative_toDir.x == all_dirs[dir_east].x && relative_toDir.y == all_dirs[dir_east].y) {
		world.pc->pos.x = 1;
		world.pc->pos.y = world.cur_map->west;
		move_char(world.pc, world.pc->pos);
	}
	else if (relative_toDir.x == all_dirs[dir_west].x && relative_toDir.y == all_dirs[dir_west].y) {
		world.pc->pos.x = MAP_X - 2;
		world.pc->pos.y = world.cur_map->east;
		move_char(world.pc, world.pc->pos);
	}
	else if (relative_toDir.x == all_dirs[dir_south].x && relative_toDir.y == all_dirs[dir_south].y) {
		world.pc->pos.x = world.cur_map->north;
		world.pc->pos.y = 1;
		move_char(world.pc, world.pc->pos);
	}
	else { // World Init PC
		Pos pos;
		do {
			rand_pos(&pos);
			// Redundant check for char_m being null since this func will be called before NPCS
		} while (mappos(pos) != ter_path || world.cur_map->char_m[pos.y][pos.x] != NULL);

		if (!world.pc) {
			world.pc = (Pc*)malloc(sizeof(*(world.cur_map->char_m)));
			world.pc->pos = (Pos) {pos.x, pos.y};
			charpos(world.pc->pos) = world.pc;
			world.pc->next_turn = 0;
			heap_insert(&world.cur_map->turn, world.pc);
		} else {
			world.pc->pos = (Pos) {pos.x, pos.y};
			move_char(world.pc, world.pc->pos);
		}
	}

}

/*
 * Does not free the PC, just removes it from the charmap
 */
void pc_remove(map_t *map) {
	int x, y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (charxy(x,y) && charxy(x,y)->type == char_pc) {
				charxy(x,y) = NULL;
				return;
			}
		}
	}
}

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
void world_changeMap(Pos to, Pos from) {

	// Index of destination map relative to departure map
	Pos relative_toDir = (Pos){to.x - from.x, to.y - from.y};

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
	world.quit = 0;
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

static void neighbor_init(Pos *neighbors, path_t *center) {
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

static void turn_neighbor_init(Pos *neighbors, Character *center) {
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
 * Places NPC characters in map according to global num_characters
 * Also places NPCs in map heap
 */
void npc_init(){
	Pos pos;
	character_type_t new_char;
	int count;
	map_t *map = world.cur_map; // Redundant, but using it for char(x,y) to work
	// decrement count till zero, adding a random character each time
	for (count = num_trainers; count > 0; count--){

		// Always place a rival and hiker when possible
		// This if block is icky lol
		if 		(count == num_trainers)	   { new_char = char_rival; }
		else if (count == num_trainers - 1){ new_char = char_hiker; }
		else 	{new_char = npc_getRandom(); }

		do {
			rand_pos(&pos);
		} while (charpos(pos) ||
				 move_cost[new_char][mappos(pos)] == INT_MAX ||
				 mappos(pos) == ter_path);

		charpos(pos) 			= (Character*)malloc(sizeof(*(map->char_m)));
		charpos(pos)->pos 		= (Pos){pos.x,pos.y};
		charpos(pos)->type 		= new_char;
		charpos(pos)->next_turn = 0;
		((Npc*)charpos(pos))->defeated = 0;
		if(new_char == char_pc) {

		}
		else if (new_char == char_statue || new_char == char_hiker  || new_char == char_rival) {
			((Npc*)charpos(pos))->dir 	= (Pos){0,0};
		} else {
			((Npc*)charpos(pos))->dir 	= all_dirs[rand() % dir_num];
		}
		heap_insert(&world.cur_map->turn, charpos(pos));
	}
}

void npc_battle(Npc *c) {
	// Battle
	int done;
	int key;
	done  = 0;
	// Right now, display to panel
	show_panel(panels[win_battle]);
	wclear(windows[win_battle]);
	while (!done) {
		box(windows[win_battle],0,0);
		mvwaddstr(windows[win_battle], 4,4,"BATTLE: Press ESC to defeat your opponent!");

		update_panels();
		doupdate();
		key = getch();
		switch (key) {
			case 27: // ESC
				c->defeated = 1;
				wclear(windows[win_battle]);
				hide_panel(panels[win_battle]);
				// PC wins
				// Mark trainer as defeated
				// Return to map
				done = 1;
				break;
		}
	}
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
	Pos relPos;
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
	Pos newPos;
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
			done = 1;
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
	while (!world.quit) {
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

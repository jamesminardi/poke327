#include "world.h"

void rand_pos(pos_t *pos) {
	pos->x = (rand() % (MAP_X - 2)) + 1;
	pos->y = (rand() % (MAP_Y - 2)) + 1;
}

void rand_dir(Npc *c) {
	int i = rand() & 0x7;
	c->dir = all_dirs[i];
}

/*
 * Compare movement cost of tile currently on
 */
static int32_t turn_cmp(const void *key, const void *with) {
	int key_moveCost = ((Character *) key)->next_turn;
	int with_moveCost = ((Character *) with)->next_turn;
	return key_moveCost - with_moveCost;
}

void heap_delete_char(void *v) {
	if (v == world.pc) {
		free(world.pc);
		//world.pc = NULL;
	} else {
		free(v);
		//v = NULL;
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

void move_char(Character *c, pos_t pos) {
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
static void pc_init(pos_t relative_toDir) {

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
		pos_t pos;
		do {
			rand_pos(&pos);
			// Redundant check for char_m being null since this func will be called before NPCS
		} while (mappos(pos) != ter_path || world.cur_map->char_m[pos.y][pos.x] != NULL);

		if (!world.pc) {
			world.pc = (Pc*)malloc(sizeof(*(world.cur_map->char_m)));
			world.pc->pos = (pos_t) {pos.x, pos.y};
			charpos(world.pc->pos) = world.pc;
			world.pc->next_turn = 0;
			heap_insert(&world.cur_map->turn, world.pc);
		} else {
			world.pc->pos = (pos_t) {pos.x, pos.y};
			move_char(world.pc, world.pc->pos);
		}
	}

}

/*
 * Does not free the PC, just removes it from the charmap
 */
static void pc_remove(map_t *map) {
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

static character_type_t npc_get_random() {
	int i, rnd;
	static int char_weight_sum = -1;

	if (char_weight_sum == -1) {
		char_weight_sum = 0;
		for (i = 1; i < num_character_types; i++) {
			char_weight_sum += char_weight[i];
		}
	}
	// algorithm to get a random character with set weights from char_weight
	rnd = rand() % char_weight_sum;
	for ( i = 1; i < num_character_types; i++) {
		if (rnd < char_weight[i]) {
			return (character_type_t)i;
		}
		rnd -= char_weight[i];
	}
	return char_unoccupied;
}


/*
 * Places NPC characters in map according to global num_characters
 * Also places NPCs in map heap
 */
static void npc_init(){
	pos_t pos;
	character_type_t new_char;
	int count;
	map_t *map = world.cur_map; // Redundant, but using it for char(x,y) to work
	// decrement count till zero, adding a random character each time
	for (count = num_trainers; count > 0; count--){

		// Always place a rival and hiker when possible
		// This if block is icky lol
		if 		(count == num_trainers)	   { new_char = char_rival; }
		else if (count == num_trainers - 1){ new_char = char_hiker; }
		else 	{new_char = npc_get_random(); }

		do {
			rand_pos(&pos);
		} while (charpos(pos) ||
				 move_cost[new_char][mappos(pos)] == INT_MAX ||
				mappos(pos) == ter_path);

		charpos(pos) 			= (Character*)malloc(sizeof(*(map->char_m)));
		charpos(pos)->pos 		= (pos_t){pos.x,pos.y};
		charpos(pos)->type 		= new_char;
		charpos(pos)->next_turn = 0;
		if(new_char == char_pc) {

		}
		else if (new_char == char_statue || new_char == char_hiker  || new_char == char_rival) {
			((Npc*)charpos(pos))->dir 	= (pos_t){0,0};
		} else {
			((Npc*)charpos(pos))->dir 	= all_dirs[rand() % dir_num];
		}
		heap_insert(&world.cur_map->turn, charpos(pos));
	}
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

void print_hiker_dist() {
	// Print hiker map
	int x,y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (world.hiker_dist[y][x] == INT_MAX) {
				printf("   ");
			} else {
				printf("%02d ", world.hiker_dist[y][x] % 100);
			}
		}
		printf("\n");
	}
}

void print_rival_dist() {
	int x,y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (world.rival_dist[y][x] == INT_MAX) {
				printf("   ");
			} else {
				printf("%02d ", world.rival_dist[y][x] % 100);
			}
		}
		printf("\n");
	}
}

static int32_t hiker_cmp(const void *key, const void *with) {
	int key_dist = world.hiker_dist[((path_t *) key)->pos.y][((path_t *) key)->pos.x];
	int with_dist = world.hiker_dist[((path_t *) with)->pos.y][((path_t *) with)->pos.x];
	return key_dist - with_dist;
}

static int32_t rival_cmp(const void *key, const void *with) {
	int key_dist = world.rival_dist[((path_t *) key)->pos.y][((path_t *) key)->pos.x];
	int with_dist = world.rival_dist[((path_t *) with)->pos.y][((path_t *) with)->pos.x];
	return key_dist - with_dist;
}

static int32_t pc_cmp(const void *key, const void *with) {
	int key_dist = world.hiker_dist[((path_t *) key)->pos.y][((path_t *) key)->pos.x];
	int with_dist = world.hiker_dist[((path_t *) with)->pos.y][((path_t *) with)->pos.x];
	return key_dist - with_dist;
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

static void pathfind_init_heap(heap_t *heap, character_type_t character) {
	switch(character) {
		case char_pc:
			heap_init(heap, pc_cmp, NULL);
			break;
		case char_hiker:
			heap_init(heap, hiker_cmp, NULL);
			break;
		case char_rival:
			heap_init(heap, rival_cmp, NULL);
			break;
		default:
			break;
	}
}

void pathfind(map_t *map, int char_dist[MAP_Y][MAP_X], const character_type_t character, const pos_t start) {
    heap_t heap;
	static path_t *c;
	static path_t distance[MAP_Y][MAP_X];
	static int initialized = 0;
	int x, y;
	int i;

	pathfind_init_heap(&heap, character);

	if (!initialized) {
		initialized = 1;
		for (y = 0; y < MAP_Y; y++) {
			for (x = 0; x < MAP_X; x++) {
				distance[y][x].pos.y = y;
				distance[y][x].pos.x = x;
			}
		}
	}

	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			char_dist[y][x] = INT_MAX;
		}
	}

	// Set PC cost to 0
	//distance[start.y][start.x].cost = 0;
	char_dist[start.y][start.x] = 0;

	// Insert all infinity cost tiles into priority queue (entire map minus boundaries)
	for (y = 1; y < MAP_Y - 1; y++) {
		for (x = 1; x < MAP_X - 1; x++) {

			if (ter_cost(x,y,character) != INT_MAX) {
				distance[y][x].hn = heap_insert(&heap, &distance[y][x]);
			} else {
				distance[y][x].hn = NULL;
			}
		}
	}

	while ((c = (path_t*)heap_remove_min(&heap))) {
		c->hn = NULL; // mark as visited
		pos_t neighbors[8];
		neighbor_init(neighbors, c);
		// For every neighbor of p, if not yet visited and cost is greater than current tile(p), then change it
		for (i = 0; i < 7; i++) {
			if ((distance[neighbors[i].y][neighbors[i].x].hn) &&
			   (char_dist[neighbors[i].y][neighbors[i].x] >
			   (char_dist[c->pos.y][c->pos.x] +
			   move_cost[character][map->m[c->pos.y][c->pos.x]])))
			{
				char_dist[neighbors[i].y][neighbors[i].x] =
						char_dist[c->pos.y][c->pos.x] +
						ter_cost(c->pos.x, c->pos.y, character);
				heap_decrease_key_no_replace(&heap, distance[neighbors[i].y][neighbors[i].x].hn);
			}
		}
	}
	heap_delete(&heap);
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

void battle(Npc *c) {
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

static void char_rivalTurn(Npc *c) {
	pos_t dest;
	int min;
	int base;
	int i;

	base = rand() & 0x7;
	dest = c->pos;
	min = INT_MAX;
	for (i = base; i < dir_num + base; i++) {
		if ((world.rival_dist[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] < min)){

			dest.x = c->pos.x + all_dirs[i & 0x7].x;
			dest.y = c->pos.y + all_dirs[i & 0x7].y;
			min = world.rival_dist[dest.y][dest.x];
		}
	}

	if (!c->defeated && charpos(dest) && charpos(dest)->type == char_pc) {
		battle(c);
	}
	if (!c->defeated && !world.cur_map->char_m[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);

}
static void char_hikerTurn(Npc *c) {
	pos_t dest;
	int min;
	int base;
	int i;

	base = rand() & 0x7;
	dest = c->pos;
	min = INT_MAX;
	for (i = base; i < dir_num + base; i++) {
		if ((world.rival_dist[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] < min)){

			dest.x = c->pos.x + all_dirs[i & 0x7].x;
			dest.y = c->pos.y + all_dirs[i & 0x7].y;
			min = world.hiker_dist[dest.y][dest.x];
		}
	}

	if (!c->defeated && charpos(dest) && charpos(dest)->type == char_pc) {
		battle(c);
	}
	if (!c->defeated && !world.cur_map->char_m[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);

}
static void char_statueTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;
	if (!world.cur_map->char_m[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
static void char_pacerTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x] !=
		 world.cur_map->m[c->pos.y][c->pos.x]) ||
		 world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		c->dir.x *= -1;
		c->dir.y *= -1;
	}

	if ((world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x] ==
		 world.cur_map->m[c->pos.y][c->pos.x]) &&
		!world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}
	move_char(c, dest);
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
static void char_wandererTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x] !=
		 world.cur_map->m[c->pos.y][c->pos.x]) ||
		 world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		rand_dir(c);
	}

	if ((world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x] ==
		 world.cur_map->m[c->pos.y][c->pos.x]) &&
		!world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}
	move_char(c, dest);
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
static void char_randomTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((move_cost[c->type][world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]] == INT_MAX) ||
		world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		c->dir.x *= -1;
		c->dir.y *= -1;
	}

	if ((move_cost[c->type][world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]] != INT_MAX) &&
		!world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}
	
	move_char(c, dest);
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
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
	pos_t newPos;

	// Flush input buffer to avoid past inputs from getting used
	done = 0;
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
				break;
				// North
			case '8':    // 8
			case 'k':    // k
				newPos = (pos_t) {c->pos.x + all_dirs[dir_north].x, c->pos.y + all_dirs[dir_north].y};
				break;
				// NorthEast
			case '9':    // 9
			case 'u':    // u
				newPos = (pos_t) {c->pos.x + all_dirs[dir_northeast].x, c->pos.y + all_dirs[dir_northeast].y};
				break;
				// West
			case '4':    // 4
			case 'h':    // h
				newPos = (pos_t) {c->pos.x + all_dirs[dir_west].x, c->pos.y + all_dirs[dir_west].y};
				break;
				// East
			case '6':    // 6
			case 'l':    // l
				newPos = (pos_t) {c->pos.x + all_dirs[dir_east].x, c->pos.y + all_dirs[dir_east].y};
				break;
				// SouthWest
			case '1':    // 1
			case 'b':    // b
				newPos = (pos_t) {c->pos.x + all_dirs[dir_southwest].x, c->pos.y + all_dirs[dir_southwest].y};
				break;
				// South
			case '2':    // 2
			case 'j':    // j
				newPos = (pos_t) {c->pos.x + all_dirs[dir_south].x, c->pos.y + all_dirs[dir_south].y};
				break;
				// SouthEast
			case '3':    // 3
			case 'n':    // n
				newPos = (pos_t) {c->pos.x + all_dirs[dir_southeast].x, c->pos.y + all_dirs[dir_southeast].y};
				break;

				// NOP
			case '5':    // 5
			case ' ':    // space
			case '.':    // .
				done = 1;
				break;

				// Enter a building
			case '>':    // >
				wclear(windows[win_top]);
				mvwaddstr(windows[win_top], 0, 0, "Entering a building not yet implemented.");
				break;

				// List of trainers
			case 't':    // t
				listTrainers();
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
		if (done) {
			return;
		}


		if (mappos(newPos) == ter_exit) {
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
			if (newPos.x < 1) { 		// W
				world_changeMap((pos_t){world.cur_idx.x - 1, world.cur_idx.y}, world.cur_idx);
			}
			world.pc->next_turn = ((Character *)heap_peek_min(&world.cur_map->turn))->next_turn;
			done = 1;
		}
		if (charpos(newPos) && ((Npc*)charpos(newPos))->defeated) {
			wclear(windows[win_top]);
			mvwaddstr(windows[win_top], 0, 0, "That trainer has already been defeated.");
		}
		if (move_cost[c->type][mappos(newPos)] != INT_MAX && charpos(newPos) && !((Npc*)charpos(newPos))->defeated && charpos(newPos) != charpos(world.pc->pos)) {
			// Battle character
			battle((Npc*)charpos(newPos));
		}
		if (move_cost[c->type][mappos(newPos)] != INT_MAX && mappos(newPos) != ter_exit && !charpos(newPos)) {
			// Move to that location
			move_char(c, newPos);
			done = 1;
		}
	} // while (!done)

	//move_char the PC if moving to new map
	// But calculate next turn and reinsert into heap before swapping maps
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
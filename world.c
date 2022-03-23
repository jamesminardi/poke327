#include "world.h"

void rand_pos(pos_t *pos) {
	pos->x = (rand() % (MAP_X - 2)) + 1;
	pos->y = (rand() % (MAP_Y - 2)) + 1;
}

void rand_dir(character_t *c) {
	int i = rand() & 0x7;
	c->dir = all_dirs[i];
}

/*
 * Compare movement cost of tile currently on
 */
static int32_t turn_cmp(const void *key, const void *with) {
	int key_moveCost = ((character_t *) key)->next_turn;
	int with_moveCost = ((character_t *) with)->next_turn;
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
	worldxy(world.cur_idx.x , world.cur_idx.y) = malloc(sizeof(*world.cur_map));
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

void move_char(character_t *c, pos_t pos) {
	// Set character position in character map
	if (pos.x != c->pos.x || pos.y != c->pos.y) {
		charpos(pos) = charpos(c->pos);
		charpos(c->pos) = NULL;
		// Set character inner position
		charpos(pos)->pos = pos;
	}
}


/*
 * Called once per world by pc_init. Places PC on random path in current map and adds to queue
 * Assumptions:
 * 		Current map is set to spawn map
 * 		Character map for current map is already malloced
 */
static void world_pcInit() {
	// Set initial pc location to random place on path

	pos_t pos;
	do {
		rand_pos(&pos);
		// Redundant check for char_m being null since this func will be called before NPCS
	} while (mappos(pos) != ter_path);
	world.pc = malloc(sizeof(*(world.cur_map->char_m)));
	world.pc->pos = (pos_t){ pos.x, pos.y};
	world.pc->dir = (pos_t){0,0};

	heap_insert(&world.cur_map->turn, world.pc);
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
	else if (relative_toDir.x == all_dirs[dir_west].x && relative_toDir.y == all_dirs[dir_west].y) {
		world.pc->pos.x = 1;
		world.pc->pos.y = world.cur_map->east;
		move_char(world.pc, world.pc->pos);
	}
	else if (relative_toDir.x == all_dirs[dir_east].x && relative_toDir.y == all_dirs[dir_east].y) {
		world.pc->pos.x = MAP_X - 2;
		world.pc->pos.y = world.cur_map->east;
		move_char(world.pc, world.pc->pos);
	}
	else if (relative_toDir.x == all_dirs[dir_south].x && relative_toDir.y == all_dirs[dir_south].y) {
		world.pc->pos.x = world.cur_map->north;
		world.pc->pos.y = 1;
		move_char(world.pc, world.pc->pos);
	}
	// Means it's flying and not a init
	else {
		// Flying
		pos_t pos;
		do {
			rand_pos(&pos);
			// Redundant check for char_m being null since this func will be called before NPCS
		} while (mappos(pos) != ter_path || world.cur_map->char_m[pos.y][pos.x] != NULL);

		if (!world.pc) {
			world.pc = malloc(sizeof(*(world.cur_map->char_m)));
			world.pc->dir = (pos_t) {0, 0};
			world.pc->pos = (pos_t) {pos.x, pos.y};
			world.pc->next_turn = 0;
			charpos(world.pc->pos) = world.pc;
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
			return i;
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

		charpos(pos) 			= malloc(sizeof(*(map->char_m)));
		charpos(pos)->pos 		= (pos_t){pos.x,pos.y};
		charpos(pos)->type 		= new_char;
		charpos(pos)->next_turn = 0;

		if (new_char == char_pc 	|| new_char == char_statue ||
			new_char == char_hiker  || new_char == char_rival) {
			charpos(pos)->dir 	= (pos_t){0,0};
		} else {
			charpos(pos)->dir 	= all_dirs[rand() % dir_num];
		}
		heap_insert(&world.cur_map->turn, charpos(pos));
	}
}

/**
 * @param to 	Index of destination map in world units
 * @param from 	Index of departure map in world units
 */
void world_changeMap(pos_t to, pos_t from) {
	map_t *old_map;

	// Index of destination map relative to departure map
	pos_t relative_toDir = (pos_t){to.x - from.x, to.y - from.y};

	if (to.x < 0 || to.x > WORLD_X - 1 || to.y < 0 || to.y > WORLD_Y - 1) {
		// ERROR: Out of bounds
		//printf("Out of bounds. Position did not change\n");
		return;
	}
//	old_map = world.cur_map; // todo Deprecated?
//	if (mv.to_dir != dir_init) {
//		pc_remove(old_map);
//	}

	// Update current map index
	world.cur_idx = to;

	// Set current map pointer, and generate the destination map if it doesn't exist yet
	if (worldxy(world.cur_idx.x, world.cur_idx.y) == NULL) {
		world_initMap();
		terrain_init(world.cur_map);
		pc_init(relative_toDir);
		heap_insert(&world.cur_map->turn, charpos(world.pc->pos));
		// Put a pointer to the PC into the destination map
		world.cur_map->char_m[world.pc->pos.y][world.pc->pos.x] = world.pc;
		world.pc->next_turn = 0;
		// Handled by game loop
		// heap_insert(&world.cur_map->turn, &world.pc);
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

	while ((c = heap_remove_min(&heap))) {
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



static void turn_neighbor_init(pos_t *neighbors, character_t *center) {
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

static void char_rivalTurn(character_t *c) {
	pos_t dest;
	int min;
	int base;
	int i;
	int touch_flag;

	base = rand() & 0x7;
	dest = c->pos;
	min = INT_MAX;
	touch_flag = 0;
	for (i = base; i < dir_num + base; i++) {
		// Destination is the smallest move cost without a character already being there
		if (world.cur_map->char_m[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] && world.cur_map->char_m[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x]->type == char_pc) {
			touch_flag = 1;
			break;
		}
		if ((world.rival_dist[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] < min) &&
			!world.cur_map->char_m[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] ){

			dest.x = c->pos.x + all_dirs[i & 0x7].x;
			dest.y = c->pos.y + all_dirs[i & 0x7].y;
			min = world.rival_dist[dest.y][dest.x];
		}
	}
	if (!world.cur_map->char_m[dest.y][dest.x] && touch_flag == 0) {
		move_char(c, dest);
	}
	touch_flag = 0;
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);

}
static void char_hikerTurn(character_t *c) {
	pos_t dest;
	int min;
	int base;
	int i;
	int touch_flag;

	base = rand() & 0x7;
	dest = c->pos;
	min = INT_MAX;
	touch_flag = 0;
	for (i = base; i < dir_num + base; i++) {
		// Destination is the smallest move cost without a character already being there
		if (world.cur_map->char_m[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] && world.cur_map->char_m[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x]->type == char_pc) {
			touch_flag = 1;
			break;
		}
		if ((world.rival_dist[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] < min) &&
			!world.cur_map->char_m[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] ){

			dest.x = c->pos.x + all_dirs[i & 0x7].x;
			dest.y = c->pos.y + all_dirs[i & 0x7].y;
			min = world.hiker_dist[dest.y][dest.x];
		}
	}
	if (!world.cur_map->char_m[dest.y][dest.x] && touch_flag == 0) {
		move_char(c, dest);
	}
	touch_flag = 0;
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
static void char_statueTurn(character_t *c) {
	pos_t dest;
	dest = c->pos;
	if (!world.cur_map->char_m[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
static void char_pacerTurn(character_t *c) {
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
static void char_wandererTurn(character_t *c) {
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
static void char_randomTurn(character_t *c) {
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



static int char_pcTurn(character_t *c) {
	char input;

	// Flush input buffer to avoid past inputs from getting used
	flushinp();
	input = getch();
	if (input == 27/*ESC*/) {
		return 1;
	}
	//move_char the PC if moving to new map
	// But calculate next turn and reinsert into heap before swapping maps
	c->next_turn += move_cost[char_pc][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
	return 0;
}

void world_updateScreen() {
	clear();
	int x, y;
	for (y = 0; y < MAP_Y; y++) {
		for(x = 0; x < MAP_X; x++) {
			if (world.cur_map->char_m[y][x]) {
				mvaddch(y+1, x, char_getSymbol(world.cur_map->char_m[y][x]->type));
			} else {
				mvaddch(y+1, x, ter_getSymbol(world.cur_map->m[y][x]));
			}
		}
	}
	refresh();
}

void world_gameLoop() {
	int quit;
	character_t *curr_char;
	pathfind(world.cur_map, world.rival_dist, char_rival, world.pc->pos);
	pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);

	quit = 0;
	world_updateScreen();
	while (heap_peek_min(&world.cur_map->turn)) {

		curr_char = ((character_t *) heap_remove_min(&world.cur_map->turn));
		switch (curr_char->type) {

			case char_pc:
				world_updateScreen();
				quit = char_pcTurn(curr_char);
				if (quit) {
					return;
				}
				pathfind(world.cur_map, world.rival_dist, char_rival, world.pc->pos);
				pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);
				break;
			case char_rival:
				char_rivalTurn(curr_char);
				break;
			case char_hiker:
				char_hikerTurn(curr_char);
				break;
			case char_statue:
				char_statueTurn(curr_char);
				break;
			case char_pacer:
				char_pacerTurn(curr_char);
				break;
			case char_wanderer:
				char_wandererTurn(curr_char);
				break;
			case char_random:
				char_randomTurn(curr_char);
				break;
			default:
				break;
		}
	}


}
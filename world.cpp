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
		world.cur_map->terM[world.cur_map->west][0] = ter_border;
	}
	if (world.cur_idx.x == WORLD_X - 1) {
		world.cur_map->terM[world.cur_map->east][MAP_X - 1] = ter_border;
	}
	if (world.cur_idx.y == 0) {
		world.cur_map->terM[0][world.cur_map->north] = ter_border;
	}
	if (world.cur_idx.y == WORLD_Y - 1) {
		world.cur_map->terM[MAP_Y - 1][world.cur_map->south] = ter_border;
	}

	// Init character map to all unoccupied and tile map to clearing
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			world.cur_map->terM[y][x] = ter_clearing;
			world.cur_map->charM[y][x] = NULL;
		}
	}
	heap_init(&world.cur_map->turn, turn_cmp, heap_delete_char);
}

void move_char(Character *c, pos_t pos) {
	// if moving the PC to itself, then check if the PC exists in the charmap somewhere else other than it's position
	// Then move that PC position to its current position
	if (c->type == char_pc && !w_charpos(c->pos) && (pos.x == c->pos.x && pos.y == c->pos.y)) {
		w_charpos(pos) = world.pc;
		world.pc->pos = pos;
	}

	if (c->type == char_pc && !w_charpos(c->pos) && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		w_charpos(pos) = world.pc;
		world.pc->pos = pos;
	}
	if (c->type == char_pc && w_charpos(c->pos) && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		w_charpos(pos) = world.pc;
		w_charpos(c->pos) = NULL;
		world.pc->pos = pos;
	}

	// Set character position in character map
	if (c->type != char_pc && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		w_charpos(pos) = w_charpos(c->pos);
		w_charpos(c->pos) = NULL;
		// Set character inner position
		w_charpos(pos)->pos = pos;
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
		} while (w_terpos(pos) != ter_path || world.cur_map->charM[pos.y][pos.x] != NULL);

		if (!world.pc) {
			world.pc = (Pc*)malloc(sizeof(*(world.cur_map->charM)));
			world.pc->pos = (pos_t) {pos.x, pos.y};
			w_charpos(world.pc->pos) = world.pc;
			world.pc->next_turn = 0;
			world.pc->pokeballs = 1;
			world.pc->revives = 1;
			world.pc->potions = 1;
			world.pc->money = 500;
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
			if (m_charxy(x, y) && m_charxy(x, y)->type == char_pc) {
				m_charxy(x, y) = NULL;
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


static void generate_npc_pokemon(Npc *c) {
	c->pokemon.push_back(new Pokemon(rand_level()));
	if (rand() % 10 < 6) {
		c->pokemon.push_back(new Pokemon(rand_level()));
		if (rand() % 10 < 6) {
			c->pokemon.push_back(new Pokemon(rand_level()));
			if (rand() % 10 < 6) {
				c->pokemon.push_back(new Pokemon(rand_level()));
				if (rand() % 10 < 6) {
					c->pokemon.push_back(new Pokemon(rand_level()));
					if (rand() % 10 < 6) {
						c->pokemon.push_back(new Pokemon(rand_level()));
					}
				}
			}
		}
	}
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
		} while (w_charpos(pos) ||
				 move_cost[new_char][w_terpos(pos)] == INT_MAX ||
				 w_terpos(pos) == ter_path);

		w_charpos(pos) 			= (Character*)malloc(sizeof(*(map->charM)));
		w_charpos(pos)->pos 		= (pos_t){pos.x, pos.y};
		w_charpos(pos)->type 		= new_char;
		w_charpos(pos)->next_turn = 0;
		((Npc*)w_charpos(pos))->defeated = 0;
		generate_npc_pokemon((Npc*)w_charpos(pos));
		w_charpos(pos)->potions = 0;
		w_charpos(pos)->pokeballs = 0;
		w_charpos(pos)->revives = 0;
		int i;
		w_charpos(pos)->money = 0;
		for (i =0; i < w_charpos(pos)->pokemon.size(); i++) {
			w_charpos(pos)->money += rand() % 100;
		}



		if(new_char == char_pc) {

		}
		else if (new_char == char_statue || new_char == char_hiker  || new_char == char_rival) {
			((Npc*)w_charpos(pos))->dir 	= (pos_t){0, 0};
		} else {
			((Npc*)w_charpos(pos))->dir 	= all_dirs[rand() % dir_num];
		}
		heap_insert(&world.cur_map->turn, w_charpos(pos));
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
		w_charpos(world.pc->pos) = NULL;
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
			   move_cost[character][map->terM[c->pos.y][c->pos.x]])))
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


/*
 * Find manhattan distance from x,y
 */
static int find_manhattan_distance(pos_t p) {
	return abs(p.x - ((WORLD_X - 1) / 2)) + abs(p.y - ((WORLD_Y - 1) / 2));
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

	if (!c->defeated && w_charpos(dest) && w_charpos(dest)->type == char_pc) {
		io_trainer_battle(c);
	}
	if (!c->defeated && !world.cur_map->charM[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][w_terpos(c->pos)];
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

	if (!c->defeated && w_charpos(dest) && w_charpos(dest)->type == char_pc) {
		io_trainer_battle(c);
	}
	if (!c->defeated && !world.cur_map->charM[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][w_terpos(c->pos)];
	heap_insert(&world.cur_map->turn, c);

}
static void char_statueTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;
	if (!world.cur_map->charM[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][w_terpos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
static void char_pacerTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((world.cur_map->terM[c->pos.y + c->dir.y][c->pos.x + c->dir.x] !=
		 world.cur_map->terM[c->pos.y][c->pos.x]) ||
		world.cur_map->charM[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		c->dir.x *= -1;
		c->dir.y *= -1;
	}

	if ((world.cur_map->terM[c->pos.y + c->dir.y][c->pos.x + c->dir.x] ==
		 world.cur_map->terM[c->pos.y][c->pos.x]) &&
		!world.cur_map->charM[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}
	move_char(c, dest);
	c->next_turn += move_cost[c->type][w_terpos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
static void char_wandererTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((world.cur_map->terM[c->pos.y + c->dir.y][c->pos.x + c->dir.x] !=
		 world.cur_map->terM[c->pos.y][c->pos.x]) ||
		world.cur_map->charM[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		rand_dir(c);
	}

	if ((world.cur_map->terM[c->pos.y + c->dir.y][c->pos.x + c->dir.x] ==
		 world.cur_map->terM[c->pos.y][c->pos.x]) &&
		!world.cur_map->charM[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}
	move_char(c, dest);
	c->next_turn += move_cost[c->type][w_terpos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
static void char_randomTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((move_cost[c->type][world.cur_map->terM[c->pos.y + c->dir.y][c->pos.x + c->dir.x]] == INT_MAX) ||
		world.cur_map->charM[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		c->dir.x *= -1;
		c->dir.y *= -1;
	}

	if ((move_cost[c->type][world.cur_map->terM[c->pos.y + c->dir.y][c->pos.x + c->dir.x]] != INT_MAX) &&
		!world.cur_map->charM[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}
	
	move_char(c, dest);
	c->next_turn += move_cost[c->type][w_terpos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}

void world_gameLoop() {
	Character *curr_char;
	pathfind(world.cur_map, world.rival_dist, char_rival, world.pc->pos);
	pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);

	io_get_starter();

	io_display_map();
	while (!world.quit) {
	// old while: heap_peek_min(&world.cur_map->turn)
		curr_char = ((Character *) heap_remove_min(&world.cur_map->turn));
		switch (curr_char->type) {

			case char_pc:
				io_display_map();
				io_player_turn();
				if (move_cost[char_hiker][w_terpos(world.pc->pos)] != INT_MAX) {
					pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);
				}
				if (move_cost[char_rival][w_terpos(world.pc->pos)] != INT_MAX) {
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
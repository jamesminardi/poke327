#include "world.h"

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

	// Init character map to all unoccupied
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			world.cur_map->m[y][x] = ter_clearing;
			world.cur_map->char_m[y][x] = NULL;
		}
	}


	terrain_init(world.cur_map);
	npc_init(world.cur_map, num_trainers);
}

/*
 * Called once per world. Places PC on random path.
 * Assumptions:
 * 		char_m for current map is already malloced
 * 		Cur_map is pointing to starting map
 */
static void world_pcInit() {
	// Set initial pc location to random place on path

	int x, y;
	do {
		x = rand() % (MAP_X - 2) + 1;
		y = rand() % (MAP_Y - 2) + 1;
		// Redundant check for char_m being null since this func will be called before NPCS
	} while (world.cur_map->m[y][x] != ter_path || world.cur_map->char_m[y][x] != NULL);
	world.pc->pos = (pos_t){ x, y};
	world.pc->dir = (pos_t){0,0};
	world.pc->next_turn = 0;
	world.pc = malloc(sizeof(*(world.cur_map->char_m)));
}

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

void world_changeMap(move_request_t mv) {
	map_t *old_map;

	if (mv.to_pos.x < 0 || mv.to_pos.x > WORLD_X - 1 || mv.to_pos.y < 0 || mv.to_pos.y > WORLD_Y - 1) {
		// ERROR: Out of bounds
		printf("Out of bounds. Position did not change\n");
		return;
	}
	old_map = world.cur_map;
	if (mv.to_dir != dir_init) {
		pc_remove(old_map);
	}

	// Update current map index
	world.cur_idx.x = mv.to_pos.x;
	world.cur_idx.y = mv.to_pos.y;

	// Set current map pointer, and gen new map if it doesn't exist yet
	if (worldxy(mv.to_pos.x, mv.to_pos.y) == NULL) {
		world_initMap();
	} else {
		// If map does exist, set current map to that map
		world.cur_map = worldxy(mv.to_pos.x, mv.to_pos.y);
	}

	// Depending on where the PC came from, put it in the right position
	switch (mv.to_dir) {
		case dir_init:
		case dir_fly:
			world_pcInit();
			break;
		case dir_north:
			world.pc->pos.x = world.cur_map->south;
			world.pc->pos.y = MAP_Y - 2;
			break;
		case dir_south:
			world.pc->pos.x = world.cur_map->north;
			world.pc->pos.y = 1;
			break;
		case dir_east:
			world.pc->pos.y = world.cur_map->west;
			world.pc->pos.x = 1;
			break;
		case dir_west:
			world.pc->pos.y = world.cur_map->east;
			world.pc->pos.x = MAP_X - 2;
			break;
		default:
			break;
	}

	// Put the PC into the new map
	world.cur_map->char_m[world.pc->pos.y][world.pc->pos.x] = world.pc;


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
	move_request_t mv;
	mv.to_pos.x = world.cur_idx.x;
	mv.to_pos.y = world.cur_idx.y;
	mv.to_dir = dir_init;
	world_changeMap(mv);
}

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

void world_print() {
	map_print(world.cur_map);
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

/*
 * Compare movement cost of tile currently on
 */
static int32_t turn_cmp(const void *key, const void *with) {
	int key_moveCost = ((character_t *) key)->next_turn;
	int with_moveCost = ((character_t *) with)->next_turn;
	return key_moveCost - with_moveCost;
}



static void char_pcTurn(character_t *c) {
}
static void char_rivalTurn(character_t *c) {
	direction_t min_dir;
	int i;
	pos_t neighbors[8];
	turn_neighbor_init(neighbors, c);


	min_dir = 0;
	for (i = 1; i < 8; i++) {
		if (world.rival_dist[neighbors[i].y][neighbors[i].x] < world.rival_dist[neighbors[min_dir].y][neighbors[min_dir].x]) {
			min_dir = i;
		}
	}
	c->dir = min_dir;
	c->next_turn = move_cost[c->type][world.cur_map->m[neighbors[min_dir].y][neighbors[min_dir].x]];

	if (world.cur_map->char_m[neighbors[c->dir].y][neighbors[c->dir].x] != NULL) {
		return;
	}
	move_char(world.cur_map, c, neighbors[c->dir]);


	// Generate next turn
	min_dir = 0;
	for (i = 1; i < 8; i++) {
		if (world.rival_dist[neighbors[i].y][neighbors[i].x] < world.rival_dist[neighbors[min_dir].y][neighbors[min_dir].x]) {
			min_dir = i;
		}
	}
	c->dir = min_dir;
	c->next_turn = move_cost[c->type][world.cur_map->m[neighbors[min_dir].y][neighbors[min_dir].x]];

}
static void char_hikerTurn(character_t *c) {
	char_rivalTurn(c);
}
static void char_statueTurn(character_t *c) {
	// Don't move
}
static void char_pacerTurn(character_t *c) {
//	direction_t min_dir;
//	int i;
//	pos_t neighbors[8];
//	turn_neighbor_init(neighbors, c);
//
//	if (world.cur_map->char_m[neighbors[c->dir].y][neighbors[c->dir].x] != NULL) {
//		return;
//	}
}
static void char_wandererTurn(character_t *c) {

}
static void char_randomTurn(character_t *c) {

}

void world_gameLoop() {
	int x,y;
	heap_t h;
	heap_init(&h, turn_cmp, NULL);

	// Insert all characters into queue
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (world.cur_map->char_m[y][x]) {
				heap_insert(&h, world.cur_map->char_m[y][x]);
			}
		}
	}

	character_t *curr_char;
	pathfind(world.cur_map, world.rival_dist, char_rival, world.pc->pos);
	pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);
	while (heap_peek_min(&h)) {

		curr_char = ((character_t *) heap_remove_min(&h));
		switch (curr_char->type) {
			case char_pc:
				world_print();
				usleep(250000);
				char_pcTurn(curr_char);
				curr_char->next_turn += move_cost[char_pc][world.cur_map->m[curr_char->pos.y][curr_char->pos.x]];
				pathfind(world.cur_map, world.rival_dist, char_rival, world.pc->pos);
				pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);
				break;
			case char_rival:
				char_rivalTurn(curr_char);
				//curr_char->next_turn += ((character_t *) heap_peek_min(&h))->next_turn;
				curr_char->next_turn += move_cost[char_pc][world.cur_map->m[curr_char->pos.y][curr_char->pos.x]];
				heap_insert(&h, curr_char);
				break;
			case char_hiker:
				char_hikerTurn(curr_char);
				//curr_char->next_turn += ((character_t *) heap_peek_min(&h))->next_turn;
				curr_char->next_turn += move_cost[char_pc][world.cur_map->m[curr_char->pos.y][curr_char->pos.x]];
				heap_insert(&h, curr_char);
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
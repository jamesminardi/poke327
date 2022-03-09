#include "world.h"

static void pc_init() {
		// Set initial pc location to random place on path
		int x, y;
		do {
			x = rand() % (MAP_X - 2) + 1;
			y = rand() % (MAP_Y - 2) + 1;
		} while (world.cur_map->m[y][x] != ter_path && world.cur_map->char_m[y][x] != char_unoccupied);
		world.pc.pos.x = x;
		world.pc.pos.y = y;
}

static void world_newMap() {
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
			world.cur_map->char_m[y][x] = char_unoccupied;
		}
	}
}

static void pc_remove(map_t *map) {
	int x, y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (charxy(x,y) == char_pc) {
				charxy(x,y) = char_unoccupied;
				return;
			}
		}
	}
}

void world_move(move_request_t mv) {
	map_t *old_map;

	if (mv.to_pos.x < 0 || mv.to_pos.x > WORLD_X - 1 || mv.to_pos.y < 0 || mv.to_pos.y > WORLD_Y - 1) {
		// ERROR: Out of bounds
		printf("Out of bounds world_move. Position did not change\n");
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
		world_newMap();
		terrain_init(world.cur_map);
		npc_init(world.cur_map, num_trainers);
	} else {
		// If map does exist, set current map to that map
		world.cur_map = worldxy(mv.to_pos.x, mv.to_pos.y);
	}

	// Depending on where the PC came from, put it in the right position
	switch (mv.to_dir) {
		case dir_init:
		case dir_fly:
			pc_init();
		case dir_north:
			world.pc.pos.x = world.cur_map->south;
			world.pc.pos.y = MAP_Y - 2;
			break;
		case dir_south:
			world.pc.pos.x = world.cur_map->north;
			world.pc.pos.y = 1;
			break;
		case dir_east:
			world.pc.pos.y = world.cur_map->west;
			world.pc.pos.x = 1;
			break;
		case dir_west:
			world.pc.pos.y = world.cur_map->east;
			world.pc.pos.x = MAP_X - 2;
			break;
		default:
			break;
	}

	// Put the PC into the new map
	world.cur_map->char_m[world.pc.pos.y][world.pc.pos.x] = char_pc;


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
	world_move(mv);
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
	int key_dist = world.hiker_dist[((path_t *) key)->pos.y][((path_t *) key)->pos.x];
	int with_dist = world.hiker_dist[((path_t *) with)->pos.y][((path_t *) with)->pos.x];
	return key_dist - with_dist;
}

static int32_t pc_cmp(const void *key, const void *with) {
	int key_dist = world.hiker_dist[((path_t *) key)->pos.y][((path_t *) key)->pos.x];
	int with_dist = world.hiker_dist[((path_t *) with)->pos.y][((path_t *) with)->pos.x];
	return key_dist - with_dist;
}

static void dijkstra_neighbor_init(pos_t *neighbors, path_t *center) {
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

static void pathfind_init_heap(heap_t *heap, character_t character) {
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

void pathfind(map_t *map, int char_dist[MAP_Y][MAP_X], const character_t character, const pos_t start) {
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
		dijkstra_neighbor_init(neighbors, c);
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
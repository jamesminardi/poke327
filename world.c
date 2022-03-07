#include "world.h"

void world_init() {
	//world.seed = time(NULL);
	world.seed = 1;
	srand(world.seed);
	world.cur_idx.x = 199;
	world.cur_idx.y = 199;

	int x;
	int y;
	for (x = 0; x < WORLD_X; x++) {
		for (y = 0; y < WORLD_Y; y++) {
			worldxy(x, y) = NULL;
		}
	}
	world_move(world.cur_idx.x, world.cur_idx.y);
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

void world_move(int x, int y) {
	if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
		// ERROR: Out of bounds
		printf("Out of bounds world_move. Position did not change\n");
		return;
	}
	world.cur_idx.x = x;
	world.cur_idx.y = y;
	if (world.cur_map == NULL) {
		world_newMap(world.cur_idx.x, world.cur_idx.y);
	}
}

void world_newMap() {

	map_t *cur_map = malloc(sizeof(*worldxy(world.cur_idx.x, world.cur_idx.y)));

	/* Set exit positions */
	if (world.cur_idx.y - 1 > 0 && worldxy(world.cur_idx.x, world.cur_idx.y - 1) != NULL) {
		worldxy(world.cur_idx.x, world.cur_idx.y)->north = worldxy(world.cur_idx.x, world.cur_idx.y - 1)->south;
	} else {
		worldxy(world.cur_idx.x, world.cur_idx.y)->north = rand() % (MAP_X - 4) + 2;
	}
	if (world.cur_idx.y + 1 < WORLD_Y && worldxy(world.cur_idx.x, world.cur_idx.y + 1) != NULL) {
		worldxy(world.cur_idx.x, world.cur_idx.y)->south = worldxy(world.cur_idx.x, world.cur_idx.y + 1)->north;
	} else {
		worldxy(world.cur_idx.x, world.cur_idx.y)->south = rand() % (MAP_X - 4) + 2;
	}
	if (world.cur_idx.x + 1 < WORLD_X && worldxy(world.cur_idx.x + 1, world.cur_idx.y) != NULL) {
		worldxy(world.cur_idx.x, world.cur_idx.y)->east = worldxy(world.cur_idx.x + 1, world.cur_idx.y)->west;
	} else {
		worldxy(world.cur_idx.x, world.cur_idx.y)->east = rand() % (MAP_Y - 4) + 2;
	}
	if (world.cur_idx.x - 1 > 0 && worldxy(world.cur_idx.x - 1, world.cur_idx.y) != NULL) {
		worldxy(world.cur_idx.x, world.cur_idx.y)->west = worldxy(world.cur_idx.x - 1, world.cur_idx.y)->east;
	} else {
		worldxy(world.cur_idx.x, world.cur_idx.y)->west = rand() % (MAP_Y - 4) + 2;
	}

	int i;
	int j;

	// Init map to clearing
	for (i = 0; i < MAP_Y; i++) {
		for (j = 0; j < MAP_X; j++) {
			world.cur_map->m[i][j] = ter_clearing;
		}
	}
	map_populate(world.cur_map);

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
}

void world_print() {
	map_print(world.cur_map);
}

static int dijkstra_cmp(const void *key, const void *with) {
	path_t tmp = *(path_t *) key;
	path_t tmp2 = *(path_t *) with;
	//return ((path_t *) key)->cost - ((path_t *) with)->cost;
	return tmp.cost - tmp2.cost;
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
	neighbors[1].x = center->pos.x - 0;

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

void pathfind(map_t *map, int char_dist[MAP_Y][MAP_X], const character_t character, const pos_t start) {
	heap_t heap;
	path_t *c;
	path_t distance[MAP_Y][MAP_X];
	int x, y;
	int i;
	switch(character) {
		case char_pc:
			heap_init(&heap, pc_cmp, NULL);
			break;
		case char_hiker:
			heap_init(&heap, hiker_cmp, NULL);
			break;
		case char_rival:
			heap_init(&heap, rival_cmp, NULL);
			break;
		default:
			break;
	}

	//dijkstra_infinity_init(distance); // Fill distance map with infinity
	//dijkstra_infinity_init(visited);
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			//distance[y][x].cost = INT_MAX;
			char_dist[y][x] = INT_MAX;
			distance[y][x].pos.y = y;
			distance[y][x].pos.x = x;
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
			if (distance[neighbors[i].y][neighbors[i].x].hn &&
			   (char_dist[neighbors[i].y][neighbors[i].x] >
			   (char_dist[c->pos.y][c->pos.x] +
			   move_cost[character][map->m[c->pos.y][c->pos.x]])))
			{
//				distance[neighbors[i].y][neighbors[i].x].cost = c->cost + map->hiker[neighbors[i].y][neighbors[i].x];
//				distance[neighbors[i].y][neighbors[i].x].from.y = c->pos.y;
//				distance[neighbors[i].y][neighbors[i].x].from.x = c->pos.x;
				char_dist[neighbors[i].y][neighbors[i].x] =
						char_dist[c->pos.y][c->pos.x] +
						ter_cost(c->pos.x, c->pos.y, character);
				heap_decrease_key_no_replace(&heap, distance[neighbors[i].y][neighbors[i].x].hn); // seg faulting in here. Can't find what I'm doing wrong
			}
		}
	}
	heap_delete(&heap);
}
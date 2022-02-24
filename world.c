#include "world.h"

void world_init() {
	//world.seed = time(NULL);
	world.seed = 1;
	srand(world.seed);
	world.pos.x = 199;
	world.pos.y = 199;

	int x;
	int y;
	for (x = 0; x < WORLD_X; x++) {
		for (y = 0; y < WORLD_Y; y++) {
			worldxy(x, y) = NULL;
		}
	}
	world_move(world.pos.x, world.pos.y);
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
	world.pos.x = x;
	world.pos.y = y;
	if (worldxy(world.pos.x, world.pos.y) == NULL) {
		world_newMap(world.pos.x, world.pos.y);
	}
}

void world_newMap() {

	worldxy(world.pos.x, world.pos.y) = malloc(sizeof(*worldxy(world.pos.x, world.pos.y)));

	/* Set exit positions */
	if (world.pos.y - 1 > 0 && worldxy(world.pos.x, world.pos.y - 1) != NULL) {
		worldxy(world.pos.x, world.pos.y)->north = worldxy(world.pos.x, world.pos.y - 1)->south;
	} else {
		worldxy(world.pos.x, world.pos.y)->north = rand() % (MAP_X - 4) + 2;
	}
	if (world.pos.y + 1 < WORLD_Y && worldxy(world.pos.x, world.pos.y + 1) != NULL) {
		worldxy(world.pos.x, world.pos.y)->south = worldxy(world.pos.x, world.pos.y + 1)->north;
	} else {
		worldxy(world.pos.x, world.pos.y)->south = rand() % (MAP_X - 4) + 2;
	}
	if (world.pos.x + 1 < WORLD_X && worldxy(world.pos.x + 1, world.pos.y) != NULL) {
		worldxy(world.pos.x, world.pos.y)->east = worldxy(world.pos.x + 1, world.pos.y)->west;
	} else {
		worldxy(world.pos.x, world.pos.y)->east = rand() % (MAP_Y - 4) + 2;
	}
	if (world.pos.x - 1 > 0 && worldxy(world.pos.x - 1, world.pos.y) != NULL) {
		worldxy(world.pos.x, world.pos.y)->west = worldxy(world.pos.x - 1, world.pos.y)->east;
	} else {
		worldxy(world.pos.x, world.pos.y)->west = rand() % (MAP_Y - 4) + 2;
	}

	int i;
	int j;

	// Init map to clearing
	for (i = 0; i < MAP_Y; i++) {
		for (j = 0; j < MAP_X; j++) {
			worldxy(world.pos.x, world.pos.y)->m[i][j] = ter_clearing;
		}
	}
	map_populate(worldxy(world.pos.x, world.pos.y));

	/* Remove road exits on edge of world */
	if (world.pos.x == 0) {
		worldxy(world.pos.x, world.pos.y)->m[worldxy(world.pos.x, world.pos.y)->west][0] = ter_border;
	}
	if (world.pos.x == WORLD_X - 1) {
		worldxy(world.pos.x, world.pos.y)->m[worldxy(world.pos.x, world.pos.y)->east][MAP_X - 1] = ter_border;
	}
	if (world.pos.y == 0) {
		worldxy(world.pos.x, world.pos.y)->m[0][worldxy(world.pos.x, world.pos.y)->north] = ter_border;
	}
	if (world.pos.y == WORLD_Y - 1) {
		worldxy(world.pos.x, world.pos.y)->m[MAP_Y - 1][worldxy(world.pos.x, world.pos.y)->south] = ter_border;
	}
}

void world_print() {
	map_print(worldxy(world.pos.x, world.pos.y));
}

static int dijkstra_cmp(const void *key, const void *with) {
	path_t tmp = *(path_t *) key;
	path_t tmp2 = *(path_t *) with;
	//return ((path_t *) key)->cost - ((path_t *) with)->cost;
	return tmp.cost - tmp2.cost;
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



void dijkstra_hiker(map_t *map, pos_t start) {
	path_t distance[MAP_Y][MAP_X];
	path_t *p;
	int x, y;
	int i;
	heap_t heap;
	heap_init(&heap, dijkstra_cmp, NULL);
	//dijkstra_infinity_init(distance); // Fill distance map with infinity
	//dijkstra_infinity_init(visited);
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			distance[y][x].cost = INT_MAX;
			distance[y][x].pos.y = y;
			distance[y][x].pos.x = x;
		}
	}
	// Set PC cost to 0
	distance[start.y][start.x].cost = 0;

	// Insert all infinity cost tiles into priority queue (entire map minus boundaries)
	for (y = 1; y < MAP_Y - 1; y++) {
		for (x = 1; x < MAP_X - 1; x++) {
			if (map->hiker[y][x] != INT_MAX) {
				distance[y][x].hn = heap_insert(&heap, &distance[y][x]);
			}
		}
	}

	while ((p = heap_remove_min(&heap))) {
		p->hn = NULL; // mark as visited
		pos_t neighbors[8];
		dijkstra_neighbor_init(neighbors, p);
		// For every neighbor of p, if not yet visited and cost is greater than current tile(p), then change it
		for (i = 0; i < 7; i++) {
			if (distance[neighbors[i].y][neighbors[i].x].hn &&
			   (distance[neighbors[i].y][neighbors[i].x].cost >
			   (p->cost + map->hiker[neighbors[i].y][neighbors[i].x])))
			{
				distance[neighbors[i].y][neighbors[i].x].cost = p->cost + map->hiker[neighbors[i].y][neighbors[i].x];
				distance[neighbors[i].y][neighbors[i].x].from.y = p->pos.y;
				distance[neighbors[i].y][neighbors[i].x].from.x = p->pos.x;
				heap_decrease_key_no_replace(&heap, distance[neighbors[i].y][neighbors[i].x].hn);
			}
		}
	}
	// Print shortest path map
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (distance[y][x].cost == INT_MAX) {
				printf("   ");
			} else {
				printf("%02d ", distance[y][x].cost % 100);
			}
		}
	}
	printf("\n");
	heap_delete(&heap);
}
#include "world.h"

void world_init() {
	world.seed = time(NULL);
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

#ifndef WORLD_H
#define WORLD_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "pos.h"
#include "globals.h"
#include "colors.h"
#include "map.h"
#include "heap.h"

#ifndef WORLD_XY
#define WORLD_XY
#define worldxy(x, y) (world.w[y][x])
#endif // WORLD_XY

#define ter_cost(x, y, c) move_cost[c][map->m[y][x]]

typedef enum character {
	char_pc,
	char_hiker,
	char_rival,
	char_other,
	num_character_types // Always right
} character_t;

static int move_cost[num_character_types][num_terrain_types] = {
		{ INT_MAX, INT_MAX, 10, 10,      10,      20, 10, INT_MAX, INT_MAX },
		{ INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 15, 10, 15,      15      },
		{ INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX },
		{ INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX },
};

typedef struct pc {
	pos_t pos;
} pc_t;

typedef struct path {
	heap_node_t *hn;
	pos_t pos;
	pos_t from;
	int cost;
} path_t;

typedef struct world {
	map_t *w[WORLD_X][WORLD_Y];
	pos_t cur_idx;
	map_t *cur_map;

	int hiker_dist[MAP_Y][MAP_X];
	int rival_dist[MAP_Y][MAP_X];
	int pc_dist[MAP_Y][MAP_X];

	pc_t pc;
	int seed;
} world_t;

extern world_t world;

void world_init();

void world_newMap();

void world_delete();

void world_move(int x, int y);

void world_print();

void pathfind(map_t *map, int char_dist[MAP_Y][MAP_X], const character_t character, const pos_t start);

#endif // WORLD_H

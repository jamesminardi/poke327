#ifndef WORLD_H
#define WORLD_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "components.h"
#include "globals.h"
#include "colors.h"
#include "map.h"
#include "heap.h"

#ifndef WORLD_XY
#define WORLD_XY
#define worldxy(x, y) (world.w[y][x])
#endif // WORLD_XY

#define ter_cost(x, y, c) move_cost[c][map->m[y][x]]

typedef struct path {
	heap_node_t *hn;
	pos_t pos;
	pos_t from;
	int cost;
} path_t;


extern world_t world;

void world_init();

void world_changeMap(move_request_t mv);

void world_delete();

void char_print();

void world_print();

void print_hiker_dist();

void print_rival_dist();

void pathfind(map_t *map, int char_dist[MAP_Y][MAP_X], const character_type_t character, const pos_t start);

void world_gameLoop();

#endif // WORLD_H

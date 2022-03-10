#ifndef MAP_H
#define MAP_H

#ifndef MAP_XY
#define MAP_XY
#define mapxy(x, y) (map->m[y][x])
#define charxy(x, y) (map->char_m[y][x])
#endif // MAP_XY

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include "components.h"
#include "globals.h"
#include "colors.h"

typedef struct map {
	terrain_t m[MAP_Y][MAP_X];
	character_t *char_m[MAP_Y][MAP_X];
	int north, south, east, west;
} map_t;

static int char_weight[num_character_types] =
		{0, 10, 25, 50, 50, 50, 25};

void move_char(map_t *map, character_t *c, pos_t pos);

void npc_init(map_t *map, int num_npc);

void terrain_init(map_t *map);

void map_print(map_t *map);

void ter_print(map_t *map);

void char_print(map_t *map);

#endif // MAP_H

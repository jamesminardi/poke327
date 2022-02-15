#ifndef WORLD_H
#define WORLD_H

#ifndef WORLD_XY
#define WORLD_XY
#define worldxy(x, y) (world.w[y][x])
#endif // WORLD_XY

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "pos.h"
#include "globals.h"
#include "terrain.h"
#include "map.h"

typedef struct world {
	map_t *w[399][399];
	pos_t pos;
	int seed;
} world_t;

extern world_t world;

void world_init();

void world_newMap();

void world_delete();

void world_move(int x, int y);

void world_print();

#endif // WORLD_H

#ifndef MAP_H
#define MAP_H

#ifndef MAP_XY
#define MAP_XY
#define mapxy(x, y) (map->m[y][x])
#endif // MAP_XY

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include "pos.h"
#include "globals.h"
#include "terrain.h"

typedef struct map {
	terrain_t m[MAP_Y][MAP_X];
	int dijk_hiker[MAP_Y][MAP_X];
	int dijk_rival[MAP_Y][MAP_X];
	int dijk_pc[MAP_Y][MAP_X];
	int north, south, east, west;
} map_t;

void map_populate(map_t *map);

void map_placeCenter(map_t *map);

void map_placeMart(map_t *map);

void find_validBuildingLocation(map_t *map, int *x, int *y);

void map_placePath(map_t *map);

void map_placeBorder(map_t *map);

void map_placeGrass(map_t *map);

void map_placeTree(map_t *map);

int is_validTree(map_t *map, int x, int y);

void map_print(map_t *map);

void map_generateCosts(map_t *map);

void map_printDijk(map_t *map);

#endif // MAP_H

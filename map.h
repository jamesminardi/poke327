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
#include "colors.h"
typedef enum terrain {
	debug, 			// 0
	empty, 			// 1
	ter_border, 	// 2
	ter_exit,		// 3
	ter_clearing, 	// 4
	ter_grass, 		// 5
	ter_path, 		// 6
	ter_boulder, 	// 7
	ter_tree, 		// 8
	ter_center, 	// 9
	ter_mart, 		// 10
	ter_mountain, 	// 11
	ter_forest, 	// 12
	ter_water, 		// 13
	num_terrain_types // 14
} terrain_t;

typedef struct map {
	enum terrain m[MAP_Y][MAP_X];
	int character_map[MAP_Y][MAP_X];
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

void map_printCostMap(int cost_map[MAP_Y][MAP_X]);

#endif // MAP_H

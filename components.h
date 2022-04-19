#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <climits>
#include "heap.h"
#include "globals.h"
#include "pos.h"


//// Do not change first 8. Locked by npc_init
//typedef enum direction { // TODO deprecated
//	dir_north,
//	dir_south,
//	dir_east,
//	dir_west,
//	dir_northeast,
//	dir_northwest,
//	dir_southeast,
//	dir_southwest,
//	dir_still,
//	dir_fly,
//	dir_init
//} direction_t;

class Character;

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

char ter_getSymbol(terrain_t t) {
	switch (t) {
		case ter_border:
			return '%';
		case ter_boulder:
			return '%';
		case ter_mountain:
			return '%';
		case ter_tree:
			return '^';
		case ter_forest:
			return '^';
		case ter_exit:
			return '#';
		case ter_path:
			return '#';
		case ter_mart:
			return 'M';
		case ter_center:
			return 'C';
		case ter_grass:
			return ':';
		case ter_clearing:
			return '.';
		case empty:
			return ' ';
		case debug:
			return ' ';
		default:
			return ' ';
	}
}

typedef struct map {
	terrain_t m[MAP_Y][MAP_X];
	Character *char_m[MAP_Y][MAP_X];
	heap_t turn;
	int north, south, east, west;
} map_t;


#endif // COMPONENTS_H

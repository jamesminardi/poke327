#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "heap.h"
#include "globals.h"


typedef struct pos {
	int x;
	int y;
} pos_t;

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


typedef enum character_type {
	char_pc, // @
	char_rival, // r
	char_hiker, // h
	char_statue, // s - dont move
	char_pacer, // p - Walk one direction and turn around
	char_wanderer, // w - Random direction, can't leave terrain
	char_random, // n - Wanderer but cross terrain
	num_character_types, // Always right
	char_unoccupied
} character_type_t;

typedef struct char_attributes {
	pos_t dir;
	int defeated;
} char_attributes_t;

typedef struct character {
	character_type_t type;
	pos_t pos;
	char_attributes_t attr;
	int next_turn;
} character_t;

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
	terrain_t m[MAP_Y][MAP_X];
	character_t *char_m[MAP_Y][MAP_X];
	heap_t turn;
	int north, south, east, west;
} map_t;

typedef struct world {
	map_t *w[WORLD_X][WORLD_Y];
	pos_t cur_idx;
	map_t *cur_map;

	int hiker_dist[MAP_Y][MAP_X];
	int rival_dist[MAP_Y][MAP_X];
	int pc_dist[MAP_Y][MAP_X];

	character_t *pc;
	int seed;

	int quit_game_flag;
} world_t;

static const int move_cost[num_character_types][num_terrain_types] = {
		{ INT_MAX, INT_MAX, INT_MAX, 10, 10, 20, 10, INT_MAX, INT_MAX, 10, 10, INT_MAX, INT_MAX, INT_MAX },
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 15, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 15, 15, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX}
};

typedef enum cardinal_dirs {
	dir_northwest,
	dir_north,
	dir_northeast,
	dir_west,
	dir_east,
	dir_southwest,
	dir_south,
	dir_southeast,
	dir_num
} cardinal_dirs_t;

static pos_t all_dirs[8] = {
		{ -1, -1 },
		{ 0,  -1 },
		{ 1,  -1 },
		{ -1, 0  },
		{ 1,  0  },
		{ -1, 1  },
		{ 0,  1  },
		{ 1,  1  },
};

#endif // COMPONENTS_H

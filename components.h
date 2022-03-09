#ifndef COMPONENTS_H
#define COMPONENTS_H

typedef struct pos {
	int x;
	int y;
} pos_t;

typedef enum direction {
	dir_north,
	dir_south,
	dir_east,
	dir_west,
	dir_fly,
	dir_init
} direction_t;

typedef struct move_request {
	direction_t to_dir;
	pos_t to_pos;
} move_request_t;


typedef enum character {
	char_pc, // @
	char_rival, // r
	char_hiker, // h
	char_statue, // s - dont move
	char_pacer, // p - Walk one direction and turn around
	char_wanderer, // w - Random direction, can't leave terrain
	char_random, // n - Wanderer but cross terrain
	num_character_types, // Always right
	char_unoccupied
} character_t;

typedef struct pc {
	pos_t pos;
} pc_t;

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

const int move_cost[num_character_types][num_terrain_types] = {
		{ INT_MAX, INT_MAX, INT_MAX, 10, 10, 20, 10, INT_MAX, INT_MAX, 10, 10, INT_MAX, INT_MAX, INT_MAX },
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 15, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 15, 15, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
};

#endif // COMPONENTS_H

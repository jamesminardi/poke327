#ifndef WORLD_H
#define WORLD_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <ncurses.h>
#include <vector>
#include <panel.h>
#include "components.h"
#include "globals.h"
#include "character.h"
#include "colors.h"
#include "map.h"
#include "heap.h"
#include "db_parse.h"

#ifndef WORLD_XY
#define WORLD_XY
#define worldxy(x, y) (world.w[y][x])
#endif // WORLD_XY

#define ter_cost(x, y, c) move_cost[c][map->m[y][x]]

//class World {
//public:
//	Map *w[WORLD_X][WORLD_Y];
//	Pos *cur_idx;
//	Map *cur_map;
//	int hiker_dist[MAP_Y][MAP_X];
//	int rival_dist[MAP_Y][MAP_X];
//	int pc_dist[MAP_Y][MAP_X];
//	Pc *pc;
//	int seed;
//	int quit_game_flag;
//	World(int seed);
//	~World();
//private:
//	Map *w[WORLD_X][WORLD_Y];
//};

typedef struct world {
	map_t *w[WORLD_X][WORLD_Y];
	pos_t cur_idx;
	map_t *cur_map;

	int hiker_dist[MAP_Y][MAP_X];
	int rival_dist[MAP_Y][MAP_X];
	int pc_dist[MAP_Y][MAP_X];

	Pc *pc;
	int seed;

	int quit_game_flag;
} world_t;

typedef struct path {
	heap_node_t *hn;
	pos_t pos;
	pos_t from;
	int cost;
} path_t;

static int char_weight[num_character_types] __attribute__((unused))=
		{0, 10, 25, 50, 50, 50, 25};

extern world_t world;
extern WINDOW *windows[num_windows];
extern PANEL *panels[num_windows];

extern pokemon_db pokemon[1093];
extern pokemon_stats_db pokemon_stats[6553];
extern pokemon_move_db pokemon_moves[528239];
extern char *types[19];
extern move_db moves[845];
extern pokemon_species_db species[899];
extern experience_db experience[601];

void world_init();

void world_changeMap(pos_t to, pos_t from);

void world_delete();

void char_print();

void world_print();

void print_hiker_dist();

void print_rival_dist();

void pathfind(map_t *map, int char_dist[MAP_Y][MAP_X], const character_type_t character, const pos_t start);

void world_gameLoop();

#endif // WORLD_H

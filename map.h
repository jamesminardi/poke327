#ifndef MAP_H
#define MAP_H

#ifndef MAP_XY
#define MAP_XY
#define mapxy(x, y) (map->m[y][x])
#define mappos(pos) (world.cur_map->m[pos.y][pos.x])
#define charxy(x, y) (map->char_m[y][x])
#define charpos(pos) (world.cur_map->char_m[pos.y][pos.x])
#endif // MAP_XY

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <string>
#include "components.h"
#include "character.h"
#include "globals.h"
#include "colors.h"
#include "pos.h"

class Map {
public:
	heap_t turn;
	int north, south, east, west;
	terrain_t terM[MAP_Y][MAP_X];
	Character* charM[MAP_Y][MAP_X];


	Map();
	Map(Pos pc_pos);
	Map(int north, int south, int east, int west);
	~Map();
	void placeCharacters(Pc* pc);
	Character* getChar(int x, int y);
	Character* getChar(Pos pos);
	void setChar(int x, int y, Character* c);
	void setChar(Pos pos, Character* c);

	terrain_t getTerrain(int x, int y);
	terrain_t getTerrain(Pos pos);
	void setTerrain(int x, int y, terrain_t ter);
	void setTerrain(Pos pos, terrain_t ter);

	void placeMart();
	void placeCenter();
	void placeBorder();
	void placeExits();
	void placePath();
	void placeGrass();
	void placeTree();

	void placePc(Pc *pc, Pos pc_pos);
	void placeNpc();
private:
	void find_validBuildingLocation(int *x, int *y);

	int is_validTree(int x, int y);
};

static Pos rand_path(Map* map) {
	Pos pos;
	do {
		rand_pos(&pos);
	} while (map->getTerrain(pos) != ter_path);
	return pos;
}

//void terrain_init(map_t *map);
//
//void map_print(map_t *map);
//
//void ter_print(map_t *map);
//
//void char_print(map_t *map);

char ter_getSymbol(terrain_t t);
char char_getSymbol(character_type_t t);
std::string char_getString(character_type_t t);
#endif // MAP_H

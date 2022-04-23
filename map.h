#ifndef MAP_H
#define MAP_H

#ifndef MAP_XY
#define MAP_XY
#define m_terxy(x, y) (map->terM[y][x])
#define m_charxy(x, y) (map->charM[y][x])
#define m_terpos(pos) (map->terM[pos.y][pos.x])
#define m_charpos(pos) (map->charM[pos.y][pos.x])

#define w_terxy(x,y) (world.cur_map->terM[y][x])
#define w_terpos(pos) (world.cur_map->terM[pos.y][pos.x])
#define w_charxy(x,y) (world.cur_map->charM[y][x])
#define w_charpos(pos) (world.cur_map->charM[pos.y][pos.x])
#define w_mapxy(x,y) (world.cur_map->charM[pos.y][pos.x])


#endif // MAP_XY

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <string>
#include "components.h"
#include "globals.h"
#include "colors.h"


void terrain_init(map_t *map);

void map_print(map_t *map);

void ter_print(map_t *map);

void char_print(map_t *map);

char ter_getSymbol(terrain_t t);
char char_getSymbol(character_type_t t);
std::string char_getString(character_type_t t);
#endif // MAP_H

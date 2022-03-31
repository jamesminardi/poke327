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
#include "components.h"
#include "globals.h"
#include "colors.h"


void terrain_init(map_t *map);

void map_print(map_t *map);

void ter_print(map_t *map);

void char_print(map_t *map);

char ter_getSymbol(terrain_t t);
char char_getSymbol(character_type_t t);
char * char_getString(character_type_t t);
#endif // MAP_H

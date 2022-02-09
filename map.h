#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "globals.h"
#include "tiles.h"


typedef struct pos {
  int x;
  int y;
} pos_t;

typedef struct map {
  tiles_t map[MAP_Y][MAP_X];
  pos_t pos;
  int north, south, east, west;
} map_t;

typedef struct world {
  map_t *world[399][399];
  pos_t pos;
  int seed;
} world_t;


void map_init(map_t *m);
void map_populate(map_t *m);
void map_grow(map_t *m);
void map_placeCM(map_t *m);
int is_validCM(TILE_SYMBOLS_t tile);
void map_placePath(map_t *m]);
void map_placeBorder(map_t *m);
void map_placeGrass(map_t *m);
void map_placeTree(map_t *m);
int is_valid_tree(map_t *m, int row, int col);
void map_print(map_t *m]);


#endif // MAP_H

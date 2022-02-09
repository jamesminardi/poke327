#ifndef WORLD_H
#define WORLD_H

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


void world_init(char world[ROWS][COLS]);
void world_populate(char world[ROWS][COLS]);
void world_grow(char world[ROWS][COLS]);
void world_placeCM(char world[ROWS][COLS]);
int is_validCM(TILE_SYMBOLS_t tile);
void world_placePath(char world[ROWS][COLS]);
void world_placeBorder(char world[ROWS][COLS]);
void world_placeGrass(char world[ROWS][COLS]);
void world_placeTree(char world[ROWS][COLS]);
int is_valid_tree(char world[ROWS][COLS], int row, int col);
void world_print(char world[ROWS][COLS]);


#endif // WORLD_H

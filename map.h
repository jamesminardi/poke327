#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "globals.h"
#include "terrain.h"


typedef struct pos {
  int x;
  int y;
} pos_t;

typedef struct map {
  terrain_t  map[MAP_Y][MAP_X];
  pos_t pos;
  int north, south, east, west;
} map_t;

typedef struct world {
  map_t *world[399][399];
  pos_t pos;
  int seed;
} world_t;

#define mapxy(x, y) (m->map[y][x])
#define worldxy(x,y) (w->world[y][x])

void world_init(world_t *w);
void world_free(world_t *w);
void world_move(world_t *w, int x, int y);
void world_print(world_t *w);



void map_init(world_t *w, int x, int y);
void map_populate(map_t *m);
void map_placeCenter(map_t *m);
void map_placeMart(map_t *m);
void find_validBuildingLocation(map_t *m, int *x, int *y);
void map_placePath(map_t *m);
void map_placeBorder(map_t *m);
void map_placeGrass(map_t *m);
void map_placeTree(map_t *m);
int is_validTree(map_t *m, int x, int y);
void map_print(map_t *m);


#endif // MAP_H

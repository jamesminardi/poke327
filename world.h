#ifndef WORLD_H
#define WORLD_H

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "tiles.h"


void world_init(char world[ROWS][COLS]);
void world_placeTerrain(char world[ROWS][COLS]);
void world_grow(char world[ROWS][COLS]);
void world_placeBorder(char world[ROWS][COLS]);
void world_placeGrass(char world[ROWS][COLS]);
void world_print(char world[ROWS][COLS]);


#endif // WORLD_H

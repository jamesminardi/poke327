#include "world.h"

void world_init()
{
  world.seed = time(NULL);
  srand(world.seed);
  world.pos.x = 199;
  world.pos.y = 199;
 
  int x = 0;
  int y = 0;
  x += 0;
  y += 0;
  for (x = 0; x < WORLD_X; x++) {
    for (y = 0; y < WORLD_Y; y++) {
      worldxy(x,y) = NULL;
    }
  }

  worldxy(199,199) = malloc(sizeof (*worldxy(world.pos.x,world.pos.y)));
  map_init(199, 199);
}

void world_delete() {

  int x = 0;
  int y = 0;
  x += 0;
  y += 0;
  for (x = 0; x < WORLD_X; x++) {
    for (y = 0; y < WORLD_Y; y++) {
      if (worldxy(x,y) != NULL) {
	
      free(world.w[y][x]);
      worldxy(x,y) = NULL;
      }
    }
  }

}

void world_move(int x, int y) {
  if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
    // ERROR: Out of bounds
    printf("Out of bounds world_move. Position did not change\n");
    return;
  }
  world.pos.x = x;
  world.pos.y = y;
  if (worldxy(world.pos.x, world.pos.y) != NULL) {
    // Map already generated
    return;
  }

  worldxy(world.pos.x,world.pos.y) = malloc(sizeof (*worldxy(world.pos.x, world.pos.y)));
  map_init(x, y);
}

void world_print() {
    map_print(worldxy(world.pos.x, world.pos.y));
}

#include <stdio.h>
#include "globals.h"
#include "world.h"

#define COLS 80
#define ROWS 21



int main(int argc, char *argv[]) {

  char world[ROWS][COLS];
  printf("Hello, World!\n\n");
  world_init(world);
  world_print(world);

  return 0;
  
}



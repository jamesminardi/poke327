#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "world.h"

#define COLS 80
#define ROWS 21



int main(int argc, char *argv[])
{
  srand(time(NULL));
  
  char world[ROWS][COLS];
  //printf("\u2588\n");
  world_init(world);
  world_print(world);

  return 0;
  
}



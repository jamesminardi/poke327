#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "map.h"

int main(int argc, char *argv[])
{
  
  world_t w;
  world_init(&w);
  world_print(&w);
  //map_init(&m);
  //map_print(&m);

  return 0;
  
}



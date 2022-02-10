#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "map.h"

int main(int argc, char *argv[])
{
  srand(time(NULL));
  
  map_t m;
  
  
  map_init(&m);
  map_print(&m);

  return 0;
  
}



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "map.h"

int main(int argc, char *argv[])
{
  
  world_t w;
  world_init(&w);
  printf("Using seed: %d\n", w.seed);
  world_print(&w);
  printf("Pos: %d, %d\n\n", w.pos.x, w.pos.y);

  
  int quit = 0;
  char input;
  int x;
  int y;
  
  while (!quit) {
  	scanf(" %c", &input);
  	switch (input) {
  	case 'n':
  		world_move(&w, w.pos.x, w.pos.y - 1);
  		break;
  	case 's':
  		world_move(&w, w.pos.x, w.pos.y + 1);
  		break;
  	case 'e':
  		world_move(&w, w.pos.x + 1, w.pos.y);
  		break;
  	case 'w':
  		world_move(&w, w.pos.x - 1, w.pos.y);
  		break;
  	case 'f':
  		scanf("%d", &x);
  		scanf("%d", &y);
  		world_move(&w, x, y);
  		break;
  	case 'q':
  		quit = 1;
  		world_free(&w);
  		break;
  	default:
  		break;
  	}
  	world_print(&w);
  	printf("Pos: %d, %d\n", w.pos.x, w.pos.y);
  }
  
  return 0;
}



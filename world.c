#include "world.h"

/*
 * Initialize world to all spaces (' ')
 * Places nodes and grows them
 * Finishes by placing the extras like centers and marts.
 */
void world_init(char world[ROWS][COLS])
{

  unsigned int row;
  unsigned int col;
  for (row = 0; row < ROWS; row++)
  {
    for (col = 0; col < COLS; col++)
    {
      world[row][col] = s_clearing_1;   
    }
  }
  world_placeGrass(world);
  world_placeBorder(world);
}

/*
 * Places all nodes on the map
 */
void world_placeTerrain(char world[ROWS][COLS])
{
  world_placeGrass(world);
  world_placeBorder(world);
}

void world_grow(char world[ROWS][COLS]) {
  unsigned int row;
  unsigned int col;
  for (row = 0; row < ROWS; row++) {
    for (col = 0; col < COLS; col++) {
    }
  }
}

void world_placeBorder(char world[ROWS][COLS]){

  int row;
  int col;

  for (row = 0; row < ROWS; row++) {
    world[row][0] = s_boulder_1;
    world[row][COLS-1] = s_boulder_1;
  }
  for (col = 0; col < COLS; col++) {
    world[0][col] = s_boulder_1;
    world[ROWS-1][col] = s_boulder_1;
  }
}

/*
 * Places grass nodes in random locations on the map
 */
void world_placeGrass(char world[ROWS][COLS])
{
  const unsigned int num_Nodes = 15;
  
  int x_pos;
  int y_pos;
  int node_count;
  int x_size;
  int y_size;
  int f;
  int g;

  for (node_count = 0; node_count < num_Nodes; node_count++)
  {
    do
    {
      x_pos = rand() % COLS;
      y_pos = rand() % ROWS;
    } while (world[y_pos][x_pos] == s_grass_1);

    x_size = rand() % 5 + 4;
    y_size = rand() % 4 + 2;
    for (f = y_pos - y_size; f < y_pos + y_size; f++) {
      for (g = x_pos - x_size; g < x_pos + x_size; g++) {
	if (f >= 0 && f < ROWS) {
	  if (g >= 0 && g < COLS) {
	    world[f][g] = s_grass_1;
	  }
	}
      }
    }
  }
}

/*
 * Prints world to terminal
 */
void world_print(char world[ROWS][COLS])
{
  unsigned int row;
  unsigned int col;
  
    for (row = 0; row < ROWS; row++)
    {
      for (col = 0; col < COLS; col++)
      {
	printf("%c", world[row][col]);
      }
      printf("\n");
    }
}

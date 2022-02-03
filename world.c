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
  world_populate(world);
}

/*
 * Places all nodes on the map
 */
void world_populate(char world[ROWS][COLS])
{
  world_placeTree(world);
  world_placeGrass(world);
  world_placeBorder(world);
  world_placePath(world);
  world_placeCM(world);
}

void world_grow(char world[ROWS][COLS]) {
  unsigned int row;
  unsigned int col;
  for (row = 0; row < ROWS; row++) {
    for (col = 0; col < COLS; col++) {
    }
  }
}

void world_placeCM(char world[ROWS][COLS]) {

  int x_pos;
  int y_pos;
  int C_placed = 0;
  while (C_placed == 0) {
    do {
      x_pos = rand() % (COLS-4) + 2;
      y_pos = rand() % (ROWS-4) + 2;
    } while (world[y_pos][x_pos] != s_path_1);

    if (is_validCM(world[y_pos+1][x_pos])) {
      if (is_validCM(world[y_pos+1][x_pos+1])) {
	if (is_validCM(world[y_pos+2][x_pos+1])){
	  if (is_validCM(world[y_pos+2][x_pos+2])){
	      world[y_pos+1][x_pos] = s_pokecenter;
	      world[y_pos+1][x_pos+1] = s_pokecenter;
	      world[y_pos+2][x_pos+1] = s_pokecenter;
	      world[y_pos+2][x_pos] = s_pokecenter;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(world[y_pos-1][x_pos])) {
      if (is_validCM(world[y_pos-1][x_pos+1])) {
	if (is_validCM(world[y_pos-2][x_pos+1])){
	  if (is_validCM(world[y_pos-2][x_pos+2])){
	      world[y_pos-1][x_pos] = s_pokecenter;
	      world[y_pos-1][x_pos+1] = s_pokecenter;
	      world[y_pos-2][x_pos+1] = s_pokecenter;
	      world[y_pos-2][x_pos] = s_pokecenter;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(world[y_pos][x_pos+1])) {
      if (is_validCM(world[y_pos+1][x_pos+1])) {
	if (is_validCM(world[y_pos+1][x_pos+2])){
	  if (is_validCM(world[y_pos][x_pos+2])){
	      world[y_pos][x_pos+1] = s_pokecenter;
	      world[y_pos+1][x_pos+1] = s_pokecenter;
	      world[y_pos+1][x_pos+2] = s_pokecenter;
	      world[y_pos][x_pos+2] = s_pokecenter;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(world[y_pos][x_pos-1])) {
      if (is_validCM(world[y_pos+1][x_pos-1])) {
	if (is_validCM(world[y_pos+1][x_pos-2])){
	  if (is_validCM(world[y_pos][x_pos-2])){
	      world[y_pos][x_pos-1] = s_pokecenter;
	      world[y_pos+1][x_pos-1] = s_pokecenter;
	      world[y_pos+1][x_pos-2] = s_pokecenter;
	      world[y_pos][x_pos-2] = s_pokecenter;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
  }

  C_placed = 0;
  while (C_placed == 0) {
    do {
      x_pos = rand() % (COLS-4) + 2;
      y_pos = rand() % (ROWS-4) + 2;
    } while (world[y_pos][x_pos] != s_path_1);

    if (is_validCM(world[y_pos+1][x_pos])) {
      if (is_validCM(world[y_pos+1][x_pos+1])) {
	if (is_validCM(world[y_pos+2][x_pos+1])){
	  if (is_validCM(world[y_pos+2][x_pos+2])){
	      world[y_pos+1][x_pos] = s_pokemart;
	      world[y_pos+1][x_pos+1] = s_pokemart;
	      world[y_pos+2][x_pos+1] = s_pokemart;
	      world[y_pos+2][x_pos] = s_pokemart;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(world[y_pos-1][x_pos])) {
      if (is_validCM(world[y_pos-1][x_pos+1])) {
	if (is_validCM(world[y_pos-2][x_pos+1])){
	  if (is_validCM(world[y_pos-2][x_pos+2])){
	      world[y_pos-1][x_pos] = s_pokemart;
	      world[y_pos-1][x_pos+1] = s_pokemart;
	      world[y_pos-2][x_pos+1] = s_pokemart;
	      world[y_pos-2][x_pos] = s_pokemart;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(world[y_pos][x_pos+1])) {
      if (is_validCM(world[y_pos+1][x_pos+1])) {
	if (is_validCM(world[y_pos+1][x_pos+2])){
	  if (is_validCM(world[y_pos][x_pos+2])){
	      world[y_pos][x_pos+1] = s_pokemart;
	      world[y_pos+1][x_pos+1] = s_pokemart;
	      world[y_pos+1][x_pos+2] = s_pokemart;
	      world[y_pos][x_pos+2] = s_pokemart;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(world[y_pos][x_pos-1])) {
      if (is_validCM(world[y_pos+1][x_pos-1])) {
	if (is_validCM(world[y_pos+1][x_pos-2])){
	  if (is_validCM(world[y_pos][x_pos-2])){
	      world[y_pos][x_pos-1] = s_pokemart;
	      world[y_pos+1][x_pos-1] = s_pokemart;
	      world[y_pos+1][x_pos-2] = s_pokemart;
	      world[y_pos][x_pos-2] = s_pokemart;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
  }
	

}

int is_validCM(TILE_SYMBOLS_t tile) {

  if (tile == s_path_1) {
    return 0;
  }
  if (tile == s_boulder_1) {
    return 0;
  }
  if (tile == s_pokecenter) {
    return 0;
  }
  if (tile == s_pokemart) {
    return 0;
  }

  return 1;

}

void world_placePath(char world[ROWS][COLS]) {

  int top_exit = rand() % (COLS - 4) + 2;
  int left_exit = rand() % (ROWS - 4) + 2;
  int right_exit = rand() % (ROWS - 4) + 2;
  int bottom_exit = rand() % (COLS - 4) + 2;
  int x_break = rand() % (COLS - 6) + 3;
  int y_break = rand() % (ROWS - 6) + 3;

  world[0][top_exit] = s_path_1;
  world[ROWS-1][bottom_exit] = s_path_1;
  world[left_exit][0] = s_path_1;
  world[right_exit][COLS-1] = s_path_1;

  int i;
  for (i = 0; i < x_break; i++) {
    world[left_exit][i] = s_path_1;
  }
  for (i = x_break; i < COLS; i++) {
    world[right_exit][i] = s_path_1;
  }
  if (left_exit < right_exit) {
    for (i = left_exit; i < right_exit; i++) {
      world[i][x_break] = s_path_1;
    }
  }
  if (left_exit >= right_exit) {
    for (i = right_exit; i <= left_exit; i++) {
      world[i][x_break] = s_path_1;
    }
  }  
  

  for (i = 0; i < y_break; i++) {
    world[i][top_exit] = s_path_1;
  }
  for (i = y_break; i < ROWS; i++) {
    world[i][bottom_exit] = s_path_1;
  }
  if (top_exit < bottom_exit) {
    for (i = top_exit; i < bottom_exit; i++) {
      world[y_break][i] = s_path_1;
    }
  }
  if (top_exit >= bottom_exit) {
    for (i = bottom_exit; i <= top_exit; i++) {
      world[y_break][i] = s_path_1;
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

void world_placeTree(char world[ROWS][COLS]) {

  int x_pos;
  int y_pos;
  int i;
  for (i = 0; i < 60; i++){

    do {
      x_pos = rand() % (COLS-2) + 1;
      y_pos = rand() % (ROWS-2) + 1;
    } while (!is_valid_tree(world, y_pos, x_pos));
    world[y_pos][x_pos] = s_tree_1;
  }

}

int is_valid_tree(char world[ROWS][COLS], int row, int col) {
  
  int valid = 1;
  
  if (row <= 0 || row >= ROWS) {
    valid = 0;
  }
  else if (col <= 0 || col >= COLS) {
    valid =  0;
  }
  
  else if (world[row][col] == s_tree_1) {
    valid =  0;
  }
  else if (world[row+1][col] == s_tree_1) {
    valid = 0;
  }
  else if (world[row-1][col] == s_tree_1) {
    valid =  0;
  }
  else if (world[row][col+1] == s_tree_1) {
    valid = 0;
  }
  else if (world[row][col-1] == s_tree_1) {
    valid = 0;
  }

  return valid;
    
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

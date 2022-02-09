#include "map.h"

/*
 * Initialize map to all spaces (' ')
 * Places nodes and grows them
 * Finishes by placing the extras like centers and marts.
 */
void map_init(map_t *m)
{

  int y;
  int x;
  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      m->map[row][col] = ter_clearing;   
    }
  }
  map_populate(map);
}

/*
 * Places all nodes on the map
 */
void map_populate(map_t *m)
{
  map_placeTree(map);
  map_placeGrass(map);
  map_placeBorder(map);
  map_placePath(map);
  map_placeCM(map);
}

void map_grow(map_t *m {
  unsigned int row;
  unsigned int col;
  for (row = 0; row < ROWS; row++) {
    for (col = 0; col < COLS; col++) {
    }
  }
}

void map_placeCM(map_t *m) {

  int x_pos;
  int y_pos;
  int C_placed = 0;
  while (C_placed == 0) {
    do {
      x_pos = rand() % (COLS-4) + 2;
      y_pos = rand() % (ROWS-4) + 2;
    } while (map[y_pos][x_pos] != s_path_1);

    if (is_validCM(map[y_pos+1][x_pos])) {
      if (is_validCM(map[y_pos+1][x_pos+1])) {
	if (is_validCM(map[y_pos+2][x_pos+1])){
	  if (is_validCM(map[y_pos+2][x_pos+2])){
	      map[y_pos+1][x_pos] = s_pokecenter;
	      map[y_pos+1][x_pos+1] = s_pokecenter;
	      map[y_pos+2][x_pos+1] = s_pokecenter;
	      map[y_pos+2][x_pos] = s_pokecenter;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(map[y_pos-1][x_pos])) {
      if (is_validCM(map[y_pos-1][x_pos+1])) {
	if (is_validCM(map[y_pos-2][x_pos+1])){
	  if (is_validCM(map[y_pos-2][x_pos+2])){
	      map[y_pos-1][x_pos] = s_pokecenter;
	      map[y_pos-1][x_pos+1] = s_pokecenter;
	      map[y_pos-2][x_pos+1] = s_pokecenter;
	      map[y_pos-2][x_pos] = s_pokecenter;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(map[y_pos][x_pos+1])) {
      if (is_validCM(map[y_pos+1][x_pos+1])) {
	if (is_validCM(map[y_pos+1][x_pos+2])){
	  if (is_validCM(map[y_pos][x_pos+2])){
	      map[y_pos][x_pos+1] = s_pokecenter;
	      map[y_pos+1][x_pos+1] = s_pokecenter;
	      map[y_pos+1][x_pos+2] = s_pokecenter;
	      map[y_pos][x_pos+2] = s_pokecenter;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(map[y_pos][x_pos-1])) {
      if (is_validCM(map[y_pos+1][x_pos-1])) {
	if (is_validCM(map[y_pos+1][x_pos-2])){
	  if (is_validCM(map[y_pos][x_pos-2])){
	      map[y_pos][x_pos-1] = s_pokecenter;
	      map[y_pos+1][x_pos-1] = s_pokecenter;
	      map[y_pos+1][x_pos-2] = s_pokecenter;
	      map[y_pos][x_pos-2] = s_pokecenter;
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
    } while (map[y_pos][x_pos] != s_path_1);

    if (is_validCM(map[y_pos+1][x_pos])) {
      if (is_validCM(map[y_pos+1][x_pos+1])) {
	if (is_validCM(map[y_pos+2][x_pos+1])){
	  if (is_validCM(map[y_pos+2][x_pos+2])){
	      map[y_pos+1][x_pos] = s_pokemart;
	      map[y_pos+1][x_pos+1] = s_pokemart;
	      map[y_pos+2][x_pos+1] = s_pokemart;
	      map[y_pos+2][x_pos] = s_pokemart;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(map[y_pos-1][x_pos])) {
      if (is_validCM(map[y_pos-1][x_pos+1])) {
	if (is_validCM(map[y_pos-2][x_pos+1])){
	  if (is_validCM(map[y_pos-2][x_pos+2])){
	      map[y_pos-1][x_pos] = s_pokemart;
	      map[y_pos-1][x_pos+1] = s_pokemart;
	      map[y_pos-2][x_pos+1] = s_pokemart;
	      map[y_pos-2][x_pos] = s_pokemart;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(map[y_pos][x_pos+1])) {
      if (is_validCM(map[y_pos+1][x_pos+1])) {
	if (is_validCM(map[y_pos+1][x_pos+2])){
	  if (is_validCM(map[y_pos][x_pos+2])){
	      map[y_pos][x_pos+1] = s_pokemart;
	      map[y_pos+1][x_pos+1] = s_pokemart;
	      map[y_pos+1][x_pos+2] = s_pokemart;
	      map[y_pos][x_pos+2] = s_pokemart;
	      C_placed = 1;
	      break;
	   }
	 }
       }
     }
    if (is_validCM(map[y_pos][x_pos-1])) {
      if (is_validCM(map[y_pos+1][x_pos-1])) {
	if (is_validCM(map[y_pos+1][x_pos-2])){
	  if (is_validCM(map[y_pos][x_pos-2])){
	      map[y_pos][x_pos-1] = s_pokemart;
	      map[y_pos+1][x_pos-1] = s_pokemart;
	      map[y_pos+1][x_pos-2] = s_pokemart;
	      map[y_pos][x_pos-2] = s_pokemart;
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

void map_placePath(map_t *m) {

  int top_exit = rand() % (COLS - 4) + 2;
  int left_exit = rand() % (ROWS - 4) + 2;
  int right_exit = rand() % (ROWS - 4) + 2;
  int bottom_exit = rand() % (COLS - 4) + 2;
  int x_break = rand() % (COLS - 6) + 3;
  int y_break = rand() % (ROWS - 6) + 3;

  map[0][top_exit] = s_path_1;
  map[ROWS-1][bottom_exit] = s_path_1;
  map[left_exit][0] = s_path_1;
  map[right_exit][COLS-1] = s_path_1;

  int i;
  for (i = 0; i < x_break; i++) {
    map[left_exit][i] = s_path_1;
  }
  for (i = x_break; i < COLS; i++) {
    map[right_exit][i] = s_path_1;
  }
  if (left_exit < right_exit) {
    for (i = left_exit; i < right_exit; i++) {
      map[i][x_break] = s_path_1;
    }
  }
  if (left_exit >= right_exit) {
    for (i = right_exit; i <= left_exit; i++) {
      map[i][x_break] = s_path_1;
    }
  }  
  

  for (i = 0; i < y_break; i++) {
    map[i][top_exit] = s_path_1;
  }
  for (i = y_break; i < ROWS; i++) {
    map[i][bottom_exit] = s_path_1;
  }
  if (top_exit < bottom_exit) {
    for (i = top_exit; i < bottom_exit; i++) {
      map[y_break][i] = s_path_1;
    }
  }
  if (top_exit >= bottom_exit) {
    for (i = bottom_exit; i <= top_exit; i++) {
      map[y_break][i] = s_path_1;
    }
  } 


  
}

void map_placeBorder(map_t *m){

  int row;
  int col;

  for (row = 0; row < ROWS; row++) {
    map[row][0] = s_boulder_1;
    map[row][COLS-1] = s_boulder_1;
  }
  for (col = 0; col < COLS; col++) {
    map[0][col] = s_boulder_1;
    map[ROWS-1][col] = s_boulder_1;
  }
}

/*
 * Places grass nodes in random locations on the map
 */
void map_placeGrass(map_t *m)
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
    } while (map[y_pos][x_pos] == s_grass_1);

    x_size = rand() % 5 + 4;
    y_size = rand() % 4 + 2;
    for (f = y_pos - y_size; f < y_pos + y_size; f++) {
      for (g = x_pos - x_size; g < x_pos + x_size; g++) {
	if (f >= 0 && f < ROWS) {
	  if (g >= 0 && g < COLS) {
	    map[f][g] = s_grass_1;
	  }
	}
      }
    }
  }
}

void map_placeTree(map_t *m) {

  int x_pos;
  int y_pos;
  int i;
  for (i = 0; i < 60; i++){

    do {
      x_pos = rand() % (COLS-2) + 1;
      y_pos = rand() % (ROWS-2) + 1;
    } while (!is_valid_tree(map, y_pos, x_pos));
    map[y_pos][x_pos] = s_tree_1;
  }

}

int is_valid_tree(map_t *m, int row, int col) {
  
  int valid = 1;
  
  if (row <= 0 || row >= ROWS) {
    valid = 0;
  }
  else if (col <= 0 || col >= COLS) {
    valid =  0;
  }
  
  else if (map[row][col] == s_tree_1) {
    valid =  0;
  }
  else if (map[row+1][col] == s_tree_1) {
    valid = 0;
  }
  else if (map[row-1][col] == s_tree_1) {
    valid =  0;
  }
  else if (map[row][col+1] == s_tree_1) {
    valid = 0;
  }
  else if (map[row][col-1] == s_tree_1) {
    valid = 0;
  }

  return valid;
    
}

/*
 * Prints mapy to terminal
 */
void map_print(map_t *m)
{
  int y;
  int x;
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      switch (m->map[y][x]) {
      case ter_boulder:
	printf(WHT "%" CRESET);
      case ter_mountain:
        printf(WHT "\u0394" CRESET);
        break;
      case ter_tree:
      case ter_forest:
        printf(GRN "^" CRESET);
        break;
      case ter_path:
        printf(YEL "#" CRESET);
        break;
      case ter_mart:
        putchar(CYN 'M' CRESET);
        break;
      case ter_center:
        putchar(HRED 'C' CRESET);
        break;
      case ter_grass:
        putchar(GRN ':' CRESET);
        break;
      case ter_clearing:
        putchar(HGRN '.' CRESET);
        break;
      case ter_block:
	printf("\u2588\n");
      default:
        break;
      }
    }
    putchar('\n');
  }
}

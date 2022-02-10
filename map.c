#include "map.h"

void world_init(world_t *w)
{
  w->seed = time(NULL);
  srand(w->seed);
  w->pos.x = 199;
  w->pos.y = 199;

  worldxy(199,199) = malloc(sizeof (*worldxy(w->pos.x,w->pos.y)));
  map_init(worldxy(w->pos.x, w->pos.y));

}

void world_print(world_t *w) {
    map_print(worldxy(w->pos.x, w->pos.y));
}






/*
 * Initialize map to all spaces (' ')
 * Places nodes and grows them
 * Finishes by placing the extras like centers and marts.
 */
void map_init(map_t *m)
{
  m->pos.x = 0;
  m->pos.y = 0;
  int y;
  int x;
  for (y = 0; y < MAP_Y; y++)
  {
    for (x = 0; x < MAP_X; x++)
    {
      m->map[y][x] = ter_clearing;   
    }
  }
  map_populate(m);
}

/*
 * Places all nodes on the map
 */
void map_populate(map_t *m)
{
  map_placeTree(m);
  map_placeGrass(m);
  map_placeBorder(m);
  map_placePath(m);
  map_placeCenter(m);
  map_placeMart(m);
}

void map_placeCenter(map_t *m) {
  int x;
  int y;
  find_validBuildingLocation(m, &x, &y);
  mapxy(x  , y  ) = ter_center;
  mapxy(x+1, y  ) = ter_center;
  mapxy(x+1, y+1) = ter_center;
  mapxy(x  , y+1) = ter_center;
}
  
void map_placeMart(map_t *m) {
  int x;
  int y;
  find_validBuildingLocation(m, &x, &y);
  mapxy(x  , y  ) = ter_mart;
  mapxy(x+1, y  ) = ter_mart;
  mapxy(x+1, y+1) = ter_mart;
  mapxy(x  , y+1) = ter_mart;
}

/*
 * Finds valid 2x2 building location coordinate.
 * Coordinate is the topleft-most unit.
 */
void find_validBuildingLocation(map_t *m, int *x, int *y) {

  do {
    *x = rand() % (MAP_X - 3) + 1;
    *y = rand() % (MAP_Y - 3) + 1;

    if ((((mapxy(*x  ,*y-1) == ter_path) && (mapxy(*x+1,*y-1) == ter_path))    ||
	 ((mapxy(*x-1,*y  ) == ter_path) && (mapxy(*x-1,*y+1) == ter_path))    ||
	 ((mapxy(*x+2,*y  ) == ter_path) && (mapxy(*x+2,*y+1) == ter_path))    ||
	 ((mapxy(*x  ,*y+2) == ter_path) && (mapxy(*x+2,*y+2) == ter_path)))   &&

	(((mapxy(*x  ,*y  ) != ter_mart) && (mapxy(*x  ,*y  ) != ter_center)   &&
	  (mapxy(*x+1,*y  ) != ter_mart) && (mapxy(*x+1,*y  ) != ter_center)   &&
	  (mapxy(*x  ,*y+1) != ter_mart) && (mapxy(*x  ,*y+1) != ter_center)   &&
	  (mapxy(*x+1,*y+1) != ter_mart) && (mapxy(*x+1,*y+1) != ter_center))) &&

	(((mapxy(*x  ,*y  ) != ter_path) &&
	  (mapxy(*x+1,*y  ) != ter_path) &&
	  (mapxy(*x  ,*y+1) != ter_path) &&
	  (mapxy(*x+1,*y+1) != ter_path)))) {
      break;
    }
  } while(1);
}

void map_placePath(map_t *m) {
  int n_exit, w_exit, e_exit, s_exit;
  int x_break, y_break;

  
  /* Find road paths that wont create a double road */
  do {
  n_exit  = rand() % (MAP_X - 4) + 2;
  w_exit  = rand() % (MAP_Y - 4) + 2;
  e_exit  = rand() % (MAP_Y - 4) + 2;
  s_exit  = rand() % (MAP_X - 4) + 2;
  x_break = rand() % (MAP_X - 6) + 3;
  y_break = rand() % (MAP_Y - 6) + 3;

  } while (abs(n_exit - x_break) == 1 || abs(s_exit - x_break == 1) ||
	   abs(w_exit - y_break) == 1 || abs(e_exit - y_break == 1));

  m->map[0]      [n_exit] = ter_path;
  m->map[MAP_Y-1][s_exit] = ter_path;
  m->map[w_exit] [0]      = ter_path;
  m->map[e_exit] [MAP_X-1]= ter_path;

  /*
   * Place roads by connecting opposite exits together with the break
   */

  /* East to West */
  int i;
  for (i = 0      ;  i < x_break; i++) { m->map[w_exit][i] = ter_path; }
  for (i = x_break;  i < MAP_X  ; i++) { m->map[e_exit][i] = ter_path; }
  
  if (w_exit < e_exit) {
    for (i = w_exit; i < e_exit ; i++) { m->map[i][x_break] = ter_path; }
  }
  if (w_exit >= e_exit) {
    for (i = e_exit; i <=w_exit ; i++) { m->map[i][x_break] = ter_path; }
  }  
  
  /* North to South */
  for (i = 0       ; i < y_break; i++) { m->map[i][n_exit]  = ter_path; }
  for (i = y_break ; i < MAP_Y  ; i++) { m->map[i][s_exit]  = ter_path; }
  if (n_exit < s_exit) {
    for (i = n_exit; i < s_exit ; i++) { m->map[y_break][i] = ter_path; }
  }
  if (n_exit >= s_exit) {
    for (i = s_exit; i <=n_exit ; i++) { m->map[y_break][i] = ter_path; }
  }   
}

void map_placeBorder(map_t *m){

  int y;
  int x;

  for (y = 0; y < MAP_Y; y++) {
    m->map[y][0]       = ter_boulder;
    m->map[y][MAP_X-1] = ter_boulder;
  }
  for (x = 0; x < MAP_X; x++) {
    m->map[0]      [x] = ter_boulder;
    m->map[MAP_Y-1][x] = ter_boulder;
  }
}

/*
 * Places grass nodes in random locations on the map
 */
void map_placeGrass(map_t *m)
{
  int x_pos;
  int y_pos;
  int x_size;
  int y_size;
  int node_count;
  int x;
  int y;

  for (node_count = 0; node_count < GRASS_NODES; node_count++)
  {
    /* Places new grass node where there isn't already grass */
    do
    {
      x_pos = rand() % MAP_X;
      y_pos = rand() % MAP_Y;
    } while (m->map[y_pos][x_pos] == ter_grass);

    x_size = rand() % 5 + 4;
    y_size = rand() % 4 + 2;

    /* Stamps grass of random size onto node */
    for (y = y_pos - y_size; y < y_pos + y_size; y++) {
      for (x = x_pos - x_size; x < x_pos + x_size; x++) {
	if (y >= 0 && y < MAP_Y) {
	  if (x >= 0 && x < MAP_X) {
	    m->map[y][x] = ter_grass;
	  }
	}
      }
    }
  }
}

void map_placeTree(map_t *m) {

  int x;
  int y;
  int n;
  for (n = 0; n < MAX_TREES; n++){

    do {
      x = rand() % (MAP_X-2) + 1;
      y = rand() % (MAP_Y-2) + 1;
    } while (!is_validTree(m, x, y));
    
    m->map[y][x] = ter_tree;
  }
}

/* 
 * Returns 0 if invalid position for a tree.
 * Single trees can not be adjacent to one another
 */
int is_validTree(map_t *m, int x, int y) {
  
  int valid = 1;
  
  if (x <= 0 || x >= MAP_X || y < 0 || y >= MAP_Y) {
    valid = 0;
  }
  
  if ((m->map[y]  [x]   == ter_tree) || (m->map[y+1][x]   == ter_tree) ||
      (m->map[y-1][x]   == ter_tree) || (m->map[y]  [x+1] == ter_tree) ||
      (m->map[y]  [x-1] == ter_tree)){
    valid =  0;
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
	printf(BOULDER "%%" CRESET);
	break;
      case ter_mountain:
        printf("\u0394" CRESET);
        break;
      case ter_tree:
      case ter_forest:
        printf(GRN "^" CRESET);
        break;
      case ter_path:
        printf(YEL "#" CRESET);
        break;
      case ter_mart:
        printf(CYN "M" CRESET);
        break;
      case ter_center:
        printf(HRED "C" CRESET);
        break;
      case ter_grass:
        printf(HGRN ":" CRESET);
        break;
      case ter_clearing:
        printf(BRIGHTGRN "." CRESET);
        break;
      case ter_block:
	printf("\u2588" CRESET);
	break;
      case empty:
	printf(" " CRESET);
      case debug:
	printf("\u058D" CRESET);
	break;
      default:
        break;
      }
    }
    putchar('\n');
  }
}

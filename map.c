#include "map.h"

void world_init(world_t *w)
{
  w->seed = time(NULL);
  srand(w->seed);
  w->pos.x = 199;
  w->pos.y = 199;

  int x = 0;
  int y = 0;
  x += 0;
  y += 0;
  for (int x = 0; x < WORLD_X; x++) {
    for (int y = 0; y < WORLD_Y; y++) {
      worldxy(x,y) = NULL;
    }
  }

  worldxy(199,199) = malloc(sizeof (*worldxy(w->pos.x,w->pos.y)));
  map_init(w, 199, 199); 
}

void world_free(world_t *w) {

  int x = 0;
  int y = 0;
  x += 0;
  y += 0;
  for (int x = 0; x < WORLD_X; x++) {
    for (int y = 0; y < WORLD_Y; y++) {
      if (w->world[y][x] != NULL) {
	
      free(w->world[y][x]);

      }
    }
  }

}

void world_move(world_t *w, int x, int y) {
  if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
    // ERROR: Out of bounds
    printf("Out of bounds world_move. Position did not change\n");
    return;
  }
  w->pos.x = x;
  w->pos.y = y;
  if (worldxy(w->pos.x, w->pos.y) != NULL) {
    // Map already generated
    return;
  }

  worldxy(w->pos.x,w->pos.y) = malloc(sizeof (*worldxy(w->pos.x, w->pos.y)));
  map_init(w, x, y);
}

void world_print(world_t *w) {
    map_print(worldxy(w->pos.x, w->pos.y));
}

/*
 * x and y is always valid
 * Initialize map to all spaces (' ')
 * Places nodes and grows them
 * Finishes by placing the extras like centers and marts.
 */
void map_init(world_t *w, int x, int y)
{
  int i;
  int j;

  /* Set exit positions */
  if (y-1 > 0 && worldxy(x,y-1) != NULL) {
    worldxy(x,y)->north = worldxy(x,y-1)->south;
  } else {
    worldxy(x,y)->north = rand() % (MAP_X - 4) + 2;
  }

  if (y+1 < WORLD_Y && worldxy(x,y+1) != NULL) {
    worldxy(x,y)->south = worldxy(x,y+1)->north;
  } else {
    worldxy(x,y)->south = rand() % (MAP_X - 4) + 2;
  }

  if (x+1 < WORLD_X && worldxy(x+1,y) != NULL) {
    worldxy(x,y)->east = worldxy(x+1,y)->west;
  } else {
    worldxy(x,y)->east = rand() % (MAP_Y - 4) + 2;
  }

  if (x-1 > 0 && worldxy(x-1,y) != NULL) {
    worldxy(x,y)->west = worldxy(x-1,y)->east;
  } else {
    worldxy(x,y)->west = rand() % (MAP_Y - 4) + 2;
  }

  for (i = 0; i < MAP_Y; i++)
  {
    for (j = 0; j < MAP_X; j++)
    {
      worldxy(x,y)->map[i][j] = ter_clearing;   
    }
  }
  map_populate(worldxy(x,y));

  if (x == 0) {
    worldxy(x,y)->map[worldxy(x,y)->west][0] = ter_boulder;
  }
  if (x == WORLD_X-1) {
    worldxy(x,y)->map[worldxy(x,y)->east][MAP_X-1] = ter_boulder;
  }
  if (y == 0) {
    worldxy(x,y)->map[0][worldxy(x,y)->north] = ter_boulder;
  }
  if (y == WORLD_Y-1) {
    worldxy(x,y)->map[MAP_Y-1][worldxy(x,y)->south] = ter_boulder;
  }
  
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
  int x_break, y_break;

  
  /* Find road paths that wont create a double road */
  do {
  x_break = rand() % (MAP_X - 6) + 3;
  y_break = rand() % (MAP_Y - 6) + 3;

  } while (abs(m->north - x_break) == 1 || abs(m->south - x_break == 1) ||
	   abs(m->west - y_break) == 1 || abs(m->east - y_break == 1));
  if (m->north != 0) {
    m->map[0][m->north] = ter_path;
  }
  if (m->south != 0) {
    m->map[MAP_Y-1][m->south] = ter_path;
  }
  if (m->west != 0) {
    m->map[m->west][0]      = ter_path;
  }
  if (m->east != 0) {
    m->map[m->east][MAP_X-1]= ter_path;
  }

  /*
   * Place roads by connecting opposite exits together with the break
   */

  /* East to West */
  int i;
  for (i = 1      ;  i < x_break ; i++) { m->map[m->west][i] = ter_path; }
  for (i = x_break;  i < MAP_X-1 ; i++) { m->map[m->east][i] = ter_path; }
  
  if (m->west < m->east) {
    for (i = m->west; i < m->east ; i++) { m->map[i][x_break] = ter_path; }
  }
  if (m->west >= m->east) {
    for (i = m->east; i <=m->west ; i++) { m->map[i][x_break] = ter_path; }
  }  
  
  /* North to South */
  for (i = 1       ; i < y_break; i++) { m->map[i][m->north]  = ter_path; }
  for (i = y_break ; i < MAP_Y-1  ; i++) { m->map[i][m->south]  = ter_path; }
  if (m->north < m->south) {
    for (i = m->north; i < m->south ; i++) { m->map[y_break][i] = ter_path; }
  }
  if (m->north >= m->south) {
    for (i = m->south; i <=m->north ; i++) { m->map[y_break][i] = ter_path; }
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

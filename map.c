#include "map.h"

/*
 * Finds valid 2x2 building location coordinate.
 * Coordinate is the top left-most unit.
 */
static void find_validBuildingLocation(map_t *map, int *x, int *y) {
	do {
		*x = rand() % (MAP_X - 3) + 1;
		*y = rand() % (MAP_Y - 3) + 1;
		if ((((mapxy(*x, *y - 1) == ter_path) && (mapxy(*x + 1, *y - 1) == ter_path)) ||
			 ((mapxy(*x - 1, *y) == ter_path) && (mapxy(*x - 1, *y + 1) == ter_path)) ||
			 ((mapxy(*x + 2, *y) == ter_path) && (mapxy(*x + 2, *y + 1) == ter_path)) ||
			 ((mapxy(*x, *y + 2) == ter_path) && (mapxy(*x + 2, *y + 2) == ter_path)))&&

			(((mapxy(*x, 	 *y)	 != ter_mart) && (mapxy(*x, 	*y) 	!= ter_center)  &&
			  (mapxy(*x + 1, *y) 	 != ter_mart) && (mapxy(*x + 1, *y) 	!= ter_center)  &&
			  (mapxy(*x, 	 *y + 1) != ter_mart) && (mapxy(*x, 	*y + 1) != ter_center)  &&
			  (mapxy(*x + 1, *y + 1) != ter_mart) && (mapxy(*x + 1, *y + 1) != ter_center)))&&

			(((mapxy(*x, 	 *y)	 != ter_path) &&
			  (mapxy(*x + 1, *y)	 != ter_path) &&
			  (mapxy(*x, 	 *y + 1) != ter_path) &&
			  (mapxy(*x + 1, *y + 1) != ter_path)))) {
			break;
		}
	} while (1);
}

static void map_placeMart(map_t *map) {
	int x;
	int y;
	find_validBuildingLocation(map, &x, &y);
	mapxy(x, 	 y) 	= ter_mart;
	mapxy(x + 1, y) 	= ter_mart;
	mapxy(x + 1, y + 1) = ter_mart;
	mapxy(x, 	 y + 1) = ter_mart;
}

static void map_placeCenter(map_t *map) {
	int x;
	int y;
	find_validBuildingLocation(map, &x, &y);
	mapxy(x, 	 y) 	= ter_center;
	mapxy(x + 1, y)	 	= ter_center;
	mapxy(x + 1, y + 1) = ter_center;
	mapxy(x, 	 y + 1) = ter_center;
}

/*
 * Places border walls with path exits
 */
static void map_placeBorder(map_t *map) {
	int y;
	int x;
	// left & right
	for (y = 0; y < MAP_Y; y++) {
		mapxy(0, y) = ter_border;
		mapxy(MAP_X - 1, y) = ter_border;
	}
	// top & bottom
	for (x = 0; x < MAP_X; x++) {
		mapxy(x, 0) = ter_border;
		mapxy(x, MAP_Y - 1) = ter_border;
	}
	mapxy(map->north, 	0) 		 	= ter_exit;
	mapxy(map->south, 	MAP_Y - 1)	= ter_exit;
	mapxy(0, 			map->west) 	= ter_exit;
	mapxy(MAP_X - 1, 	map->east)	= ter_exit;
}

/*
 * Does not place path on border
 */
static void map_placePath(map_t *map) {
	int x_break, y_break;

	/* Find road paths that wont create a double road */
	do {
		x_break = rand() % (MAP_X - 6) + 3;
		y_break = rand() % (MAP_Y - 6) + 3;

	} while (abs(map->north - x_break) == 1 || abs(map->south - x_break == 1) ||
			 abs(map->west  - y_break) == 1 || abs(map->east  - y_break == 1));

	/* Place roads by connecting opposite exits together with the break */
	/* East to West */
	int i;
	for (i = 1; 	  i < x_break; 	 i++) { mapxy(i, map->west) = ter_path; }
	for (i = x_break; i < MAP_X - 1; i++) { mapxy(i, map->east) = ter_path; }

	if (map->west <  map->east) {
		for (i = map->west; i <  map->east; i++) { mapxy(x_break, i) = ter_path; }
	}
	if (map->west >= map->east) {
		for (i = map->east; i <= map->west; i++) { mapxy(x_break, i) = ter_path; }
	}
	/* North to South */
	for (i = 1; 	  i < y_break; 	 i++) { mapxy(map->north, i) = ter_path; }
	for (i = y_break; i < MAP_Y - 1; i++) { mapxy(map->south, i) = ter_path; }
	if (map->north <  map->south) {
		for (i = map->north; i <  map->south; i++) { mapxy(i, y_break) = ter_path; }
	}
	if (map->north >= map->south) {
		for (i = map->south; i <= map->north; i++) { mapxy(i, y_break) = ter_path; }
	}
}

/*
 * Places grass nodes in random locations on the map
 */
static void map_placeGrass(map_t *map) {
	int x_pos, y_pos, x_size, y_size;
	int node_count;
	int x, y;

	for (node_count = 0; node_count < GRASS_NODES; node_count++) {
		/* Places new grass node where there isn't already grass */
		do {
			x_pos = rand() % MAP_X;
			y_pos = rand() % MAP_Y;
		} while (mapxy(x_pos, y_pos) == ter_grass);

		x_size = rand() % 5 + 4;
		y_size = rand() % 4 + 2;

		/* Stamps grass of random size onto node */
		for (y = y_pos - y_size; y < y_pos + y_size; y++) {
			for (x = x_pos - x_size; x < x_pos + x_size; x++) {
				if (y >= 0 && y < MAP_Y) {
					if (x >= 0 && x < MAP_X) {
						mapxy(x, y) = ter_grass;
					}
				}
			}
		}
	}
}

/*
 * Returns 0 if invalid position for a tree.
 * Single trees can not be adjacent to one another or forests
 */
static int is_validTree(map_t *map, int x, int y) {

	int valid = 1;

	if (x <= 0 || x >= MAP_X || y < 0 || y >= MAP_Y) {
		valid = 0;
	}

	if ((mapxy(x, y) 	 	 == ter_tree)	|| (mapxy(x, y + 1) 	== ter_tree) ||
		(mapxy(x, y - 1) 	 == ter_tree)	|| (mapxy(x + 1, y) 	== ter_tree) ||
		(mapxy(x - 1, y) 	 == ter_tree)	|| (mapxy(x - 1, y - 1) == ter_tree) ||
		(mapxy(x + 1, y - 1) == ter_tree)	|| (mapxy(x - 1, y + 1) == ter_tree) ||
		(mapxy(x + 1, y + 1) == ter_tree)) {
		valid = 0;
	}
	if ((mapxy(x, y) 	 	 == ter_forest)	|| (mapxy(x, y + 1) 	== ter_forest) ||
		(mapxy(x, y - 1) 	 == ter_forest)	|| (mapxy(x + 1, y) 	== ter_forest) ||
		(mapxy(x - 1, y) 	 == ter_forest)	|| (mapxy(x - 1, y - 1) == ter_forest) ||
		(mapxy(x + 1, y - 1) == ter_forest)	|| (mapxy(x - 1, y + 1) == ter_forest) ||
		(mapxy(x + 1, y + 1) == ter_forest)) {
		valid = 0;
	}
	return valid;
}

static void map_placeTree(map_t *map) {

	int x, y, n;
	for (n = 0; n < MAX_TREES; n++) {

		do {
			x = rand() % (MAP_X - 2) + 1;
			y = rand() % (MAP_Y - 2) + 1;
		} while (!is_validTree(map, x, y));

		mapxy(x, y) = ter_tree;
	}
}

/*
 * Places all nodes on the map
 */
void terrain_init(map_t *map) {
	// Terrain and buildings
	map_placeTree(map);
	map_placeGrass(map);
	map_placePath(map);
	map_placeBorder(map);
	map_placeCenter(map);
	map_placeMart(map);
}


char ter_getSymbol(terrain_t t) {
	switch (t) {
		case ter_border:
			return '%';
		case ter_boulder:
			return '%';
		case ter_mountain:
			return '%';
		case ter_tree:
			return '^';
		case ter_forest:
			return '^';
		case ter_exit:
			return '#';
		case ter_path:
			return '%';
		case ter_mart:
			return 'M';
		case ter_center:
			return 'C';
		case ter_grass:
			return ':';
		case ter_clearing:
			return '.';
		case empty:
			return ' ';
		case debug:
			return ' ';
		default:
			return ' ';
	}
}

char char_getSymbol(character_type_t t) {
	switch (t) {
		case char_pc:
			return '@';
		case char_rival:
			return 'r';
		case char_hiker:
			return 'h';
		case char_statue:
			return 's';
		case char_pacer:
			return 'p';
		case char_wanderer:
			return 'w';
		case char_random:
			return 'r';
		case char_unoccupied:
		default:
			return '-';
	}
}
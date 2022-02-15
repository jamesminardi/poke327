#include "map.h"

/*
 * Places all nodes on the map
 */
void map_populate(map_t *map) {
	map_placeTree(map);
	map_placeGrass(map);
	map_placeBorder(map);
	map_placePath(map);
	map_placeCenter(map);
	map_placeMart(map);
}

void map_placeCenter(map_t *map) {
	int x;
	int y;
	find_validBuildingLocation(map, &x, &y);
	mapxy(x, y) = ter_center;
	mapxy(x + 1, y) = ter_center;
	mapxy(x + 1, y + 1) = ter_center;
	mapxy(x, y + 1) = ter_center;
}

void map_placeMart(map_t *map) {
	int x;
	int y;
	find_validBuildingLocation(map, &x, &y);
	mapxy(x, y) = ter_mart;
	mapxy(x + 1, y) = ter_mart;
	mapxy(x + 1, y + 1) = ter_mart;
	mapxy(x, y + 1) = ter_mart;
}

/*
 * Finds valid 2x2 building location coordinate.
 * Coordinate is the top left-most unit.
 */
void find_validBuildingLocation(map_t *map, int *x, int *y) {

	do {
		*x = rand() % (MAP_X - 3) + 1;
		*y = rand() % (MAP_Y - 3) + 1;

		if ((((mapxy(*x, *y - 1) == ter_path) && (mapxy(*x + 1, *y - 1) == ter_path)) ||
			 ((mapxy(*x - 1, *y) == ter_path) && (mapxy(*x - 1, *y + 1) == ter_path)) ||
			 ((mapxy(*x + 2, *y) == ter_path) && (mapxy(*x + 2, *y + 1) == ter_path)) ||
			 ((mapxy(*x, *y + 2) == ter_path) && (mapxy(*x + 2, *y + 2) == ter_path))) &&

			(((mapxy(*x, *y) != ter_mart) && (mapxy(*x, *y) != ter_center) &&
			  (mapxy(*x + 1, *y) != ter_mart) && (mapxy(*x + 1, *y) != ter_center) &&
			  (mapxy(*x, *y + 1) != ter_mart) && (mapxy(*x, *y + 1) != ter_center) &&
			  (mapxy(*x + 1, *y + 1) != ter_mart) && (mapxy(*x + 1, *y + 1) != ter_center))) &&

			(((mapxy(*x, *y) != ter_path) &&
			  (mapxy(*x + 1, *y) != ter_path) &&
			  (mapxy(*x, *y + 1) != ter_path) &&
			  (mapxy(*x + 1, *y + 1) != ter_path)))) {
			break;
		}
	} while (1);
}

void map_placePath(map_t *map) {
	int x_break, y_break;


	/* Find road paths that wont create a double road */
	do {
		x_break = rand() % (MAP_X - 6) + 3;
		y_break = rand() % (MAP_Y - 6) + 3;

	} while (abs(map->north - x_break) == 1 || abs(map->south - x_break == 1) ||
			 abs(map->west - y_break) == 1 || abs(map->east - y_break == 1));
	if (map->north != 0) {
		mapxy(map->north, 0) = ter_path;
	}
	if (map->south != 0) {
		mapxy(map->south, MAP_Y - 1) = ter_path;
	}
	if (map->west != 0) {
		mapxy(0, map->west) = ter_path;
	}
	if (map->east != 0) {
		mapxy(MAP_X - 1, map->east) = ter_path;
	}

	/*
	 * Place roads by connecting opposite exits together with the break
	 */

	/* East to West */
	int i;
	for (i = 1; i < x_break; i++) { mapxy(i, map->west) = ter_path; }
	for (i = x_break; i < MAP_X - 1; i++) { mapxy(i, map->east) = ter_path; }

	if (map->west < map->east) {
		for (i = map->west; i < map->east; i++) { mapxy(x_break, i) = ter_path; }
	}
	if (map->west >= map->east) {
		for (i = map->east; i <= map->west; i++) { mapxy(x_break, i) = ter_path; }
	}

	/* North to South */
	for (i = 1; i < y_break; i++) { mapxy(map->north, i) = ter_path; }
	for (i = y_break; i < MAP_Y - 1; i++) { mapxy(map->south, i) = ter_path; }
	if (map->north < map->south) {
		for (i = map->north; i < map->south; i++) { mapxy(i, y_break) = ter_path; }
	}
	if (map->north >= map->south) {
		for (i = map->south; i <= map->north; i++) { mapxy(i, y_break) = ter_path; }
	}
}

void map_placeBorder(map_t *map) {

	int y;
	int x;

	for (y = 0; y < MAP_Y; y++) {
		mapxy(0, y) = ter_boulder;
		mapxy(MAP_X - 1, y) = ter_boulder;
	}
	for (x = 0; x < MAP_X; x++) {
		mapxy(x, 0) = ter_boulder;
		mapxy(x, MAP_Y - 1) = ter_boulder;
	}
}

/*
 * Places grass nodes in random locations on the map
 */
void map_placeGrass(map_t *map) {
	int x_pos;
	int y_pos;
	int x_size;
	int y_size;
	int node_count;
	int x;
	int y;

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

void map_placeTree(map_t *map) {

	int x;
	int y;
	int n;
	for (n = 0; n < MAX_TREES; n++) {

		do {
			x = rand() % (MAP_X - 2) + 1;
			y = rand() % (MAP_Y - 2) + 1;
		} while (!is_validTree(map, x, y));

		mapxy(x, y) = ter_tree;
	}
}

/* 
 * Returns 0 if invalid position for a tree.
 * Single trees can not be adjacent to one another
 */
int is_validTree(map_t *map, int x, int y) {

	int valid = 1;

	if (x <= 0 || x >= MAP_X || y < 0 || y >= MAP_Y) {
		valid = 0;
	}

	if ((mapxy(x, y) == ter_tree) || (mapxy(x, y + 1) == ter_tree) ||
		(mapxy(x, y - 1) == ter_tree) || (mapxy(x + 1, y) == ter_tree) ||
		(mapxy(x - 1, y) == ter_tree)) {
		valid = 0;
	}
	return valid;
}

/*
 * Prints mapy to terminal
 */
void map_print(map_t *map) {
	int y;
	int x;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			switch (mapxy(x, y)) {
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

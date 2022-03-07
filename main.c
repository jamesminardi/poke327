#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "world.h"
#include "map.h"

world_t world;

int main(int argc, char *argv[]) {

	world_init();
	world_print();
	printf("Using seed: %d\n", world.seed);

	// Init pc
	int x, y;
	do {
		x = rand() % (MAP_X - 2) + 1;
		y = rand() % (MAP_Y - 2) + 1;
	} while (world.cur_map->m[y][x] != ter_path);
	world.pc.pos.x = x;
	world.pc.pos.y = y;

	pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc.pos);

	// Print hiker map
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (world.rival_dist[y][x] == INT_MAX) {
				printf("   ");
			} else {
				printf("%02d ", world.rival_dist[y][x] % 100);
			}
		}
	}


//	char input;
//	int x;
//	int y;
//
//	do {
//		printf("\n");
//		world_print();
//		printf("Current position is (%d,%d).  "
//			   "Enter command: ",
//			   world.pos.x,
//			   world.pos.y);
//
//		scanf(" %c", &input);
//		switch (input) {
//			case 'd':
//				map_generateCosts(worldxy(world.pos.x,world.pos.y));
//				map_printDijk(worldxy(world.pos.x,world.pos.y));
//				break;
//			case 'n':
//				world_move(world.pos.x, world.pos.y - 1);
//				break;
//			case 's':
//				world_move(world.pos.x, world.pos.y + 1);
//				break;
//			case 'e':
//				world_move(world.pos.x + 1, world.pos.y);
//				break;
//			case 'w':
//				world_move(world.pos.x - 1, world.pos.y);
//				break;
//			case 'f':
//				scanf("%d", &x);
//				scanf("%d", &y);
//				world_move(x, y);
//				break;
//			case '?':
//			case 'h':
//				printf("Move with 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
//					   "Quit with 'q'.  '?' and 'h' print this help message.\n");
//				break;
//			default:
//				break;
//		}
//	} while (input != 'q');
	world_delete();
	return 0;
}



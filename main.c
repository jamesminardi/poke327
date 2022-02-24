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
	pos_t pc;
	pc.y = 15;
	pc.x = 30;
	map_generateCosts(worldxy(world.pos.x,world.pos.y));
	map_printCostMap(worldxy(world.pos.x,world.pos.y)->hiker);
	dijkstra_hiker(worldxy(world.pos.x,world.pos.y), pc);
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



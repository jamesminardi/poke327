#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "globals.h"
#include "world.h"

world_t world;

int main(int argc, char *argv[]) {


	world_init();
	printf("Using seed: %d\n", world.seed);

	char input;
	int x;
	int y;

	do{
		printf("\n");
		world_print();
		printf("Current position is (%d,%d).  "
			   "Enter command: ",
			   world.pos.x,
			   world.pos.y);

		scanf(" %c", &input);
		switch (input) {
			case 'n':
				world_move(world.pos.x, world.pos.y - 1);
				break;
			case 's':
				world_move(world.pos.x, world.pos.y + 1);
				break;
			case 'e':
				world_move(world.pos.x + 1, world.pos.y);
				break;
			case 'w':
				world_move(world.pos.x - 1, world.pos.y);
				break;
			case 'f':
				scanf("%d", &x);
				scanf("%d", &y);
				world_move(x, y);
				break;
			default:
				break;
		}
		//world_print();
		//printf("Pos: %d, %d\n", world.pos.x, world.pos.y);
	} while (input != 'q');
	world_delete();
	return 0;
}



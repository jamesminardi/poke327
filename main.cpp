#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <ncurses.h>
#include <panel.h>
#include "globals.h"
#include "world.h"
#include "map.h"



//typedef struct option {
//	const char *name; 	// Name without -- in front
//	int has_arg; 		// One of: no_argument, required_argument, optional_argument
//	int *flag;			// How results are returned
//	int val;			// Value to return or put in flag
//} option_t;

world_t world;
WINDOW *windows[num_windows];
PANEL *panels[num_windows];
int num_trainers;

static void print_help(char* title) {
	printf("Poke327.\n\nUsage: %s [options]\n\n", title);
	printf("Options:\n");
	printf("  -h --help\t\tShow this screen.\n");
	printf("  --setseed=<s>\t\tSet world seed {0..} [default: random].\n");
	printf("  --numtrainers=<n>\tSet number of trainers per map {0..100} [default: 10].\n");
}

/*
 * Returns 1 to terminate program
 */
static int argument_handler(int argc, char *argv[]) {
	int c;
	int quit = 0;
	int seed_flag = 0;
	int trainers_flag = 0;
	while(1) {
		int option_index = 0;
		static struct option long_options[] = {
				{"help", 				  no_argument, 	NULL, 'h'},
				{"setseed",		required_argument, 	NULL, 's'},
				{"numtrainers",	required_argument,	NULL, 't'},
				{ NULL,							0,	NULL,  0 }
		};
		c = getopt_long(argc, argv, "h", long_options, &option_index);
		if (c == -1) {
			break;
		}

		switch(c) {
			// SEED
			case's': world.seed = atoi(optarg);
				if (world.seed < MIN_SEED) {
					printf("Option --setseed requires argument {0..}.\n");
					print_help(argv[0]);
					quit = 1;
					break;
				}
				printf("Using set seed: %d\n", world.seed);
				seed_flag = 1;
				break;

			// NUM_TRAINERS
			case 't': num_trainers = atoi(optarg);
				if (num_trainers < MIN_TRAINERS || num_trainers > MAX_TRAINERS) {
					printf("Option --numtrainers requires argument {0..100}.\n");
					print_help(argv[0]);
					quit = 1;
					break;
				}
				printf("Using set num_trainers: %d\n", num_trainers);
				trainers_flag = 1;
				break;

			// HELP / INVALID
			case 'h':
			default:
				print_help(argv[0]);
				quit = 1;
				break;
		}
		if (quit) {
			return -1;
		}
	}


	if (seed_flag == 0) {
		world.seed = time(NULL);
		printf("Using random seed: %d\n", world.seed);
	}
	if (trainers_flag == 0) {
		num_trainers = DEFAULT_TRAINERS;
		printf("Using default num_trainers: %d\n", num_trainers);
	}

	return 0;
}

static void io_init_terminal() {
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
}

void wins_init(){
	windows[win_map] = newwin(MAP_Y, TERMINAL_X, 1, 0);
	windows[win_top] = newwin(1, TERMINAL_X, 0, 0);
	windows[win_bottom] = newwin(2, TERMINAL_X, TERMINAL_Y - 2, 0);
	windows[win_battle] = newwin(MAP_Y-4, TERMINAL_X-4, 3, 2);
	windows[win_trainers] = newwin(MAP_Y-4, TERMINAL_X-20, 3, 10);

	box(windows[win_map], 0, 0);
	box(windows[win_bottom], 0, 0);
	box(windows[win_battle],0,0);
	box(windows[win_trainers],0,0);
}

int main(int argc, char *argv[]) {

	//db_parse(true);

	return 0;

//	//int seed, trainers;
//	int quit;
//	if ((quit = argument_handler(argc, argv))){
//		return quit;
//	}
//	io_init_terminal();
//	wins_init();
//
//	/* Attach a panel to each window */ 	/* Order is bottom up */
//	panels[win_bottom] = new_panel(windows[win_bottom]);
//	panels[win_map] = new_panel(windows[win_map]);
//	panels[win_top] = new_panel(windows[win_top]);
//	panels[win_battle] = new_panel(windows[win_battle]);
//	panels[win_trainers] = new_panel(windows[win_trainers]);
//
//	hide_panel(panels[win_battle]);
//	hide_panel(panels[win_trainers]);
//
//	update_panels(); // Write panels to vitual screen in correct visibility order
//	doupdate(); // Shows to screen
//	world_init();
//	world_gameLoop();
//	world_delete();
//
//
//	endwin();
//	return 0;



	//pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc.pos);
	//pathfind(world.cur_map, world.rival_dist, char_hiker, world.pc.pos);

	//print_hiker_dist();
	//print_rival_dist();

//	char input;
//	int x, y;
//	move_request_t mv;
//
//	do {
//		printf("\n");
//		world_print();
//		printf("Current position is (%d,%d).  "
//			   "Enter command: ",
//			   world.cur_idx.x,
//			   world.cur_idx.y);
//
//		scanf(" %c", &input);
//		switch (input) {
//			case 'd':
//				pathfind(world.cur_map, world.hiker_dist, char_hiker, world.pc->pos);
//				print_hiker_dist();
//				break;
//			case 'n':
//				mv.to_pos.x = world.cur_idx.x;
//				mv.to_pos.y = world.cur_idx.y - 1;
//				mv.to_dir = dir_north;
//				world_move(mv);
//				break;
//			case 's':
//				mv.to_pos.x = world.cur_idx.x;
//				mv.to_pos.y = world.cur_idx.y + 1;
//				mv.to_dir = dir_south;
//				world_move(mv);
//				break;
//			case 'e':
//				mv.to_pos.x = world.cur_idx.x + 1;
//				mv.to_pos.y = world.cur_idx.y;
//				mv.to_dir = dir_east;
//				world_move(mv);
//				break;
//			case 'w':
//				mv.to_pos.x = world.cur_idx.x - 1;
//				mv.to_pos.y = world.cur_idx.y;
//				mv.to_dir = dir_west;
//				world_move(mv);
//				break;
//			case 'f':
//				scanf("%d", &x);
//				scanf("%d", &y);
//				mv.to_pos.x = x;
//				mv.to_pos.y = y;
//				mv.to_dir = dir_fly;
//				world_move(mv);
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
}



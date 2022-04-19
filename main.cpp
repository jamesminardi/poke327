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
#include "db_parse.h"
#include "io.h"



//typedef struct option {
//	const char *name; 	// Name without -- in front
//	int has_arg; 		// One of: no_argument, required_argument, optional_argument
//	int *flag;			// How results are returned
//	int val;			// Value to return or put in flag
//} option_t;

World* world;

int num_trainers;
int seed;

static void print_help(char* title) {
	printf("Poke327.\n\nUsage: %s [options]\n\n", title);
	printf("Options:\n");
	printf("  -h --help\t\tShow this screen.\n");
	printf("  --setseed=<s>\t\tSet world seed {0..} [default: random].\n");
	printf("  --numtrainers=<n>\tSet number of trainers per map {0..150} [default: 10].\n");
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
			case's': seed = atoi(optarg);
				if (seed < MIN_SEED) {
					printf("Option --setseed requires argument {0..}.\n");
					print_help(argv[0]);
					quit = 1;
					break;
				}
				printf("Using set seed: %d\n", seed);
				seed_flag = 1;
				break;

			// NUM_TRAINERS
			case 't': num_trainers = atoi(optarg);
				if (num_trainers < MIN_TRAINERS || num_trainers > MAX_TRAINERS) {
					printf("Option --numtrainers requires argument {0..150}.\n");
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
		seed = time(NULL);
		printf("Using random seed: %d\n", seed);
	}
	if (trainers_flag == 0) {
		num_trainers = DEFAULT_TRAINERS;
		printf("Using default num_trainers: %d\n", num_trainers);
	}

	return 0;
}

int main(int argc, char *argv[]) {

	//int seed, trainers;
	int quit;
	if ((quit = argument_handler(argc, argv))){
		return quit;
	}

	db_parse(false);

	io_init();

	world = new World(seed, num_trainers);
	world->gameLoop();
	world_init();
	world_gameLoop();
	world_delete();
	delete world;

	io_end();

	return 0;
}



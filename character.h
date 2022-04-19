#ifndef POKE327_CHARACTER_H
#define POKE327_CHARACTER_H

#include <climits>
#include "pos.h"
#include "globals.h"
#include "components.h"
#include "heap.h"
#include "world.h"
#include "map.h"

typedef enum character_type {
	char_pc, // @
	char_rival, // r
	char_hiker, // h
	char_statue, // s - dont move
	char_pacer, // p - Walk one direction and turn around
	char_wanderer, // w - Random direction, can't leave terrain
	char_random, // n - Wanderer but cross terrain
	num_character_types, // Always right
	char_unoccupied
} character_type_t;


extern const int move_cost[num_character_types][num_terrain_types];

class Character {
public:
	Character(character_type_t type, int x, int y, int next_turn);

	character_type_t type;
	Pos pos;
	int next_turn;
	const char toSymbol();
	const char* toString();
};

class Pc : public Character {
public:
	Pc();
	void move(Map* map, Pos pos);
	void move(Map* map, int x, int y);
};

class Npc : public Character {
public:
	Npc(character_type_t type, int x, int y, int next_turn);
	int defeated;
	Pos dir;
	void rand_dir();
	void move(Map* map, int x, int y);
	void move(Map* map, Pos pos);

	void rivalTurn(Map* map);
	void hikerTurn(Map* map;
	void statueTurn(Map* map);
	void pacerTurn(Map* map);
	void wandererTurn(Map* map);
	void randomTurn(Map* map);
};

static character_type_t npc_getRandom() {
	int i, rnd;
	static int char_weight_sum = -1;

	if (char_weight_sum == -1) {
		char_weight_sum = 0;
		for (i = 1; i < num_character_types; i++) {
			char_weight_sum += char_weight[i];
		}
	}
	// algorithm to get a random character with set weights from char_weight
	rnd = rand() % char_weight_sum;
	for ( i = 1; i < num_character_types; i++) {
		if (rnd < char_weight[i]) {
			return (character_type_t)i;
		}
		rnd -= char_weight[i];
	}
	return char_unoccupied;
}

int32_t turn_cmp(const void *key, const void *with);
void heap_delete_char(void *v);

static int32_t hiker_cmp(const void *key, const void *with) {
	int key_dist = world.hiker_dist[((path_t *) key)->pos.y][((path_t *) key)->pos.x];
	int with_dist = world.hiker_dist[((path_t *) with)->pos.y][((path_t *) with)->pos.x];
	return key_dist - with_dist;
}

static int32_t rival_cmp(const void *key, const void *with) {
	int key_dist = world.rival_dist[((path_t *) key)->pos.y][((path_t *) key)->pos.x];
	int with_dist = world.rival_dist[((path_t *) with)->pos.y][((path_t *) with)->pos.x];
	return key_dist - with_dist;
}

static int32_t pc_cmp(const void *key, const void *with) {
	int key_dist = world.hiker_dist[((path_t *) key)->pos.y][((path_t *) key)->pos.x];
	int with_dist = world.hiker_dist[((path_t *) with)->pos.y][((path_t *) with)->pos.x];
	return key_dist - with_dist;
}

void pathfind_init_heap(heap_t *heap, character_type_t character) {
	switch(character) {
		case char_pc:
			heap_init(heap, pc_cmp, NULL);
			break;
		case char_hiker:
			heap_init(heap, hiker_cmp, NULL);
			break;
		case char_rival:
			heap_init(heap, rival_cmp, NULL);
			break;
		default:
			break;
	}
}

void pathfind(map_t *map, int char_dist[MAP_Y][MAP_X], const character_type_t character, const pos_t start);

void move_char(Character *c, pos_t pos);

character_type_t npc_getRandom();

//void char_rivalTurn(Npc *c);
//void char_hikerTurn(Npc *c);
//void char_statueTurn(Npc *c);
//void char_pacerTurn(Npc *c);
//void char_wandererTurn(Npc *c);
//void char_randomTurn(Npc *c);

void print_hiker_dist() {
	// Print hiker map
	int x,y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (world.hiker_dist[y][x] == INT_MAX) {
				printf("   ");
			} else {
				printf("%02d ", world.hiker_dist[y][x] % 100);
			}
		}
		printf("\n");
	}
}

void print_rival_dist() {
	int x,y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (world.rival_dist[y][x] == INT_MAX) {
				printf("   ");
			} else {
				printf("%02d ", world.rival_dist[y][x] % 100);
			}
		}
		printf("\n");
	}
}

#endif //POKE327_CHARACTER_H

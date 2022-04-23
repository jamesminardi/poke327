#ifndef POKE327_CHARACTER_H
#define POKE327_CHARACTER_H

#include <vector>
#include "Pos.h"
#include "pokemon.h"

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

class Character {
public:
	character_type_t type;
	pos_t pos;
	int next_turn;
	std::vector<Pokemon*> pokemon;
	char getSymbol();
	const char* getName();
	void addPokemon(Pokemon* pokemon);
	void removePokemon(int i);

	Pokemon *firstAvailablePokemon(); // Nullptr possible

	int pokeballs;
	int revives;
	int potions;
};

class Pc : public Character {
public:
};

class Npc : public Character {
public:
	int defeated;
	pos_t dir;
};


#endif //POKE327_CHARACTER_H

#include "character.h"

char Character::getSymbol() {
	switch (this->type) {
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
			return 'n';
		case char_unoccupied:
		default:
			return '-';
	}
}

const char* Character::getName() {
	switch (this->type) {
		case char_pc:
			return "Player Character";
		case char_rival:
			return "Rival Trainer";
		case char_hiker:
			return "Hiker Trainer";
		case char_statue:
			return "Stationary Trainer";
		case char_pacer:
			return "Pacer Trainer";
		case char_wanderer:
			return "Wanderer Trainer";
		case char_random:
			return "Random Walker Trainer";
		case char_unoccupied:
		default:
			return "-";
	}
}

void Character::addPokemon(Pokemon *pk) {
	this->pokemon.push_back(pk);
}

void Character::removePokemon(int i) {
	this->pokemon.erase(pokemon.begin() + i);
}

Pokemon *Character::firstAvailablePokemon() {
	// Find first valid pokemon
	int i;
	for (i=0; i < pokemon.size(); i++) {
		if (pokemon.at(i)->get_hp() > 0) {
			return pokemon.at(i);
		}
	}
	return nullptr;
}

#ifndef POKE327_POKEMON_H
#define POKE327_POKEMON_H

# include <iostream>

enum pokemon_stat {
	stat_hp,
	stat_atk,
	stat_def,
	stat_spatk,
	stat_spdef,
	stat_speed
};

enum pokemon_gender {
	gender_female,
	gender_male
};

class Pokemon {
private:
	int level;
	int pokemon_index;
	int move_index[4];
	int pokemon_species_index;
	int iv[6];
	int effective_stat[6];
	int shiny;
	pokemon_gender gender;

public:
	Pokemon(int level);
	const char *get_species() const;
	int get_hp() const;
	int get_atk() const;
	int get_def() const;
	int get_spatk() const;
	int get_spdef() const;
	int get_speed() const;
	const char *get_gender_string() const;
	bool is_shiny() const;
	const char *get_move(int i) const;
	std::ostream &print(std::ostream &o) const;
};

std::ostream &operator<<(std::ostream &o, const Pokemon &p);


#endif //POKE327_POKEMON_H

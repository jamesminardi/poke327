#ifndef POKE327_POKEMON_H
#define POKE327_POKEMON_H

#include <iostream>
#include <vector>

enum pokemon_stat {
	stat_hp,
	stat_atk,
	stat_def,
	stat_spatk,
	stat_spdef,
	stat_speed
};

enum gender_t {
	male,
	female
};

class Pokemon {
private:
	int level;
	int pokemon_index;
	int pokemon_species_index;
	int iv[6];
	int effective_stat[6];
	int shiny;
	int hp;
	gender_t gender;

public:
	Pokemon(int level);
	int move_index[4];
	std::vector<int> type_index;
	int base_speed;
	const char *get_species() const;
	int get_max_hp() const;
	int get_atk() const;
	int get_def() const;
	int get_spatk() const;
	int get_spdef() const;
	int get_speed() const;
	int get_hp() const;
	void set_hp(const int newHp);
	void take_damage(const int dmg);
	const char *get_gender_string() const;
	const char *get_type(int i) const;
	void add_type(int type);
	bool is_shiny() const;
	const char *get_move(int i) const;
	const int get_num_moves() const;
	std::ostream &print(std::ostream &o) const;

	int get_level() const;

	const char *get_shiny_string() const;
};

std::ostream &operator<<(std::ostream &o, const Pokemon &p);


#endif //POKE327_POKEMON_H
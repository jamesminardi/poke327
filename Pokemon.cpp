#include "Pokemon.h"
#include "db_parse.h"

static int compare_move(const void *v1, const void *v2) {
	return ((levelup_move *) v1)->level - ((levelup_move *) v2)->level;
}

Pokemon::Pokemon(int level) : level(level) {
	pokemon_species_db *s;
	unsigned i,j;
	int found;
}

const char *Pokemon::get_species() const
{
	return species[pokemon_species_index].identifier;
}

int Pokemon::get_hp() const
{
	return effective_stat[stat_hp];
}

int Pokemon::get_atk() const
{
	return effective_stat[stat_atk];
}

int Pokemon::get_def() const
{
	return effective_stat[stat_def];
}

int Pokemon::get_spatk() const
{
	return effective_stat[stat_spatk];
}

int Pokemon::get_spdef() const
{
	return effective_stat[stat_spdef];
}

int Pokemon::get_speed() const
{
	return effective_stat[stat_speed];
}

const char *Pokemon::get_gender_string() const
{
	return gender == gender_female ? "female" : "male";
}

bool Pokemon::is_shiny() const
{
	return shiny;
}

const char *Pokemon::get_move(int i) const
{
	if (i < 4 && move_index[i]) {
		return moves[move_index[i]].identifier;
	} else {
		return "";
	}
}

std::ostream &Pokemon::print(std::ostream &o) const
{
	pokemon_species_db *s = species + pokemon_species_index;
	unsigned i;

	o << get_species() << " level:" << level << " "
	  << get_gender_string() << " " << (shiny ? "shiny" : "not shiny")
	  << std::endl;
	o << "         HP:" << effective_stat[stat_hp] << std::endl
	  << "        ATK:" << effective_stat[stat_atk] << std::endl
	  << "        DEF:" << effective_stat[stat_def] << std::endl
	  << "      SPATK:" << effective_stat[stat_spatk] << std::endl
	  << "      SPDEF:" << effective_stat[stat_spdef] << std::endl
	  << "      SPEED:" << effective_stat[stat_speed] << std::endl;
	o << "      ATKIV:" << iv[stat_atk] << std::endl
	  << "      DEFIV:" << iv[stat_def] << std::endl
	  << "    SPATKIV:" << iv[stat_spatk] << std::endl
	  << "    SPDEFIV:" << iv[stat_spdef] << std::endl
	  << "    SPEEDIV:" << iv[stat_speed] << std::endl;
	o << "     HPBASE:" << s->base_stat[stat_hp] << std::endl
	  << "    ATKBASE:" << s->base_stat[stat_atk] << std::endl
	  << "    DEFBASE:" << s->base_stat[stat_def] << std::endl
	  << "  SPATKBASE:" << s->base_stat[stat_spatk] << std::endl
	  << "  SPDEFBASE:" << s->base_stat[stat_spdef] << std::endl
	  << "  SPEEDBASE:" << s->base_stat[stat_speed] << std::endl;

	o << "  Levelup moves: " << std::endl;
	for (i = 0; i < s->num_levelup_moves; i++) {
		o << "    " << moves[s->levelup_moves[i].move].identifier
		  << ":" << s->levelup_moves[i].level << std::endl;
	}
	o << "  Known moves: " << std::endl;
	if (move_index[0]) {
		o << "    " << moves[move_index[0]].identifier << std::endl;
	}
	if (move_index[1]) {
		o << "    " << moves[move_index[1]].identifier << std::endl;
	}
	if (move_index[2]) {
		o << "    " << moves[move_index[2]].identifier << std::endl;
	}
	if (move_index[3]) {
		o << "    " << moves[move_index[3]].identifier << std::endl;
	}

	return o;
}

/*
 * Overload << operator
 */
std::ostream &operator<<(std::ostream &o, const Pokemon &p)
{
	return p.print(o);
}

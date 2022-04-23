#include "pokemon.h"
#include "db_parse.h"

static int compare_move(const void *v1, const void *v2) {
	return ((levelup_move *) v1)->level - ((levelup_move *) v2)->level;
}

Pokemon::Pokemon(int level) : level(level) {
	pokemon_species_db *s;
	unsigned i,j;
	int found;

	// Subtract 1 because array is 1-indexed
	pokemon_species_index = rand() % ((sizeof (species) /
									   sizeof (species[0])) - 1);
	s = species + pokemon_species_index;

	// Set Types
	for (i = 0; type_index.size() < 2 && i < (sizeof(pokemon_types) / sizeof(pokemon_types[0])); i++) {
		if (species[pokemon_species_index].id == pokemon_types[i].pokemon_id) {
			type_index.push_back(pokemon_types[i].type_id);
		}
	}

	if (!s->levelup_moves) {
		// We have never generated a pokemon of this species before, so we
		// need to find it's level-up moveset and save it for next time.
		for (s->num_levelup_moves = 0, i = 1;
			 i < (sizeof (pokemon_moves) / sizeof (pokemon_moves[0]));
			 i++) {
			if (s->id == pokemon_moves[i].pokemon_id &&
				pokemon_moves[i].pokemon_move_method_id == 1) {
				for (found = false, j = 0; !found && j < s->num_levelup_moves; j++) {
					if (s->levelup_moves[j].move == pokemon_moves[i].move_id) {
						found = true;
					}
				}
				if (!found) {
					s->num_levelup_moves++;
					s->levelup_moves = ((levelup_move *)
							realloc(s->levelup_moves,
									(s->num_levelup_moves *
									 sizeof (*s->levelup_moves))));
					s->levelup_moves[s->num_levelup_moves - 1].level =
							pokemon_moves[i].level;
					s->levelup_moves[s->num_levelup_moves - 1].move =
							pokemon_moves[i].move_id;
				}
			}
		}
		// s->levelup_moves now contains all of the moves this species can learn
		// through leveling up.  Now we'll sort it by level to make that process
		// simpler.
		qsort(s->levelup_moves, s->num_levelup_moves,
			  sizeof (*s->levelup_moves), compare_move);

		// Also initialize base stats while we're here
		s->base_stat[0] = pokemon_stats[pokemon_species_index * 6 - 5].base_stat;
		s->base_stat[1] = pokemon_stats[pokemon_species_index * 6 - 4].base_stat;
		s->base_stat[2] = pokemon_stats[pokemon_species_index * 6 - 3].base_stat;
		s->base_stat[3] = pokemon_stats[pokemon_species_index * 6 - 2].base_stat;
		s->base_stat[4] = pokemon_stats[pokemon_species_index * 6 - 1].base_stat;
		s->base_stat[5] = pokemon_stats[pokemon_species_index * 6 - 0].base_stat;
	}

	// Get pokemon's move(s).
	for (i = 0;
		 i < s->num_levelup_moves && s->levelup_moves[i].level <= level;
		 i++)
		;

	// 0 is an invalid index, since the array is 1 indexed.
	move_index[0] = move_index[1] = move_index[2] = move_index[3] = 0;
	// I don't think 0 moves is possible, but account for it to be safe
	if (i) {
		move_index[0] = s->levelup_moves[rand() % i].move;
		if (i != 1) {
			do {
				j = rand() % i;
			} while (s->levelup_moves[j].move == move_index[0]);
			move_index[1] = s->levelup_moves[j].move;
		}
	}

	// Calculate IVs
	for (i = 0; i < 6; i++) {
		iv[i] = rand() & 0xf;
		effective_stat[i] = 5 + ((s->base_stat[i] + iv[i]) * 2 * level) / 100;
		if (i == 0) { // HP
			effective_stat[i] += 5 + level;
		}
	}
	base_speed = s->base_stat[stat_speed];

	shiny = (rand() % 8192 == 0);
	gender = ((rand() % 2 == 1) ? female : male);
	hp = effective_stat[stat_hp];
}

const char *Pokemon::get_species() const
{
	return species[pokemon_species_index].identifier;
}

int Pokemon::get_level() const
{
	return level;
}

int Pokemon::get_max_hp() const
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
	return gender == female ? "female" : "male";
}

bool Pokemon::is_shiny() const
{
	return shiny;
}
const char *Pokemon::get_shiny_string() const
{
	return shiny == true ? "shiny" : "natural";
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
	  << std::endl
	  << get_type(0) << " " << get_type(1)
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

const char *Pokemon::get_type(int i) const {
	if(i == 0) {
		return types[type_index[0]];
	}
	if (i==1) {
		if (type_index.size() > 1) {
			return types[type_index[1]];
		} else{
			return "";
		}
	}
	return "";
}

int Pokemon::get_hp() const {
	return this->hp;
}

void Pokemon::set_hp(const int newHp) {
	int new_hp = newHp;
	if (new_hp > get_max_hp()) {
		new_hp = get_max_hp();
	}
	if (new_hp < 0) {
		new_hp = 0;
	}
	this->hp = new_hp;
}

void Pokemon::add_type(int type) {
	this->type_index.push_back(type);
}

void Pokemon::take_damage(const int dmg) {
	set_hp(hp - dmg);
}

const int Pokemon::get_num_moves() const {
	int i;
	for (i = 0; i < 4 && move_index[i]; i++) {}
	return i;
}

/*
 * Overload << operator
 */
std::ostream &operator<<(std::ostream &o, const Pokemon &p)
{
	return p.print(o);
}
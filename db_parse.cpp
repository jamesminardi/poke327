#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>

#include "db_parse.h"

static char *next_token(char *start, char delim)
{
	int i;
	static char *s;

	if (start) {
		s = start;
	}

	start = s;

	for (i = 0; s[i] && s[i] != delim; i++)
		;
	s[i] = '\0';
	s = s + i + 1;

	return start;
}

pokemon_move_db pokemon_moves[528239];
pokemon_db pokemon[1093];
char *types[19];
move_db moves[845];
pokemon_species_db species[899];
experience_db experience[601];

void db_parse(bool print)
{
	FILE *f;
	char line[800];
	int i;
	char *tmp;
	struct stat buf;
	char *prefix;
	int prefix_len;
	int j;
	int count;

	i = (strlen(getenv("HOME")) +
		 strlen("/.poke327/pokedex/pokedex/data/csv/") + 1);
	prefix = (char *) malloc(i);
	strcpy(prefix, getenv("HOME"));
	strcat(prefix, "/.poke327/pokedex/pokedex/data/csv/");

	if (stat(prefix, &buf)) {
		free(prefix);
		prefix = NULL;
	}

	if (!prefix && !stat("/share/cs327", &buf)) {
		prefix = strdup("/share/cs327/pokedex/pokedex/data/csv/");
	} else if (!prefix) {
		// Your third location goes here, if needed.
		// prefix is freed later, so be sure you malloc it
	}

	//No error checking on file load from here on out.  Missing
	//files are "user error".
	prefix_len = strlen(prefix);

	prefix = (char *) realloc(prefix, prefix_len + strlen("pokemon.csv") + 1);
	strcpy(prefix + prefix_len, "pokemon.csv");

	f = fopen(prefix, "r");

	//No null byte copied here, so prefix is not technically a string anymore.
	prefix = (char *) realloc(prefix, prefix_len + 1);

	fgets(line, 80, f);

	for (i = 1; i <= 1092; i++) {
		fgets(line, 80, f);
		pokemon[i].id = atoi(next_token(line, ','));
		strncpy(pokemon[i].identifier, next_token(NULL, ','), 30);
		pokemon[i].species_id = atoi(next_token(NULL, ','));
		pokemon[i].height = atoi(next_token(NULL, ','));
		pokemon[i].weight = atoi(next_token(NULL, ','));
		pokemon[i].base_experience = atoi(next_token(NULL, ','));
		pokemon[i].order = atoi(next_token(NULL, ','));
		pokemon[i].is_default = atoi(next_token(NULL, ','));
	}

	fclose(f);

	if (print) {
		for (i = 0; i < 1092; i++) {
			printf("%d %s %d %d %d %d %d %d\n", pokemon[i].id, pokemon[i].identifier,
				   pokemon[i].species_id, pokemon[i].height, pokemon[i].weight,
				   pokemon[i].base_experience, pokemon[i].order, pokemon[i].is_default);
		}
	}


	prefix = (char *) realloc(prefix, prefix_len + strlen("moves.csv") + 1);
	strcpy(prefix + prefix_len, "moves.csv");

	f = fopen(prefix, "r");

	//No null byte copied here, so prefix is not technically a string anymore.
	prefix = (char *) realloc(prefix, prefix_len + 1);

	fgets(line, 800, f);

	for (i = 1; i <= 844; i++) {
		fgets(line, 800, f);
		moves[i].id = atoi((tmp = next_token(line, ',')));
		strcpy(moves[i].identifier, (tmp = next_token(NULL, ',')));
		tmp = next_token(NULL, ',');
		moves[i].generation_id = *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].type_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].power =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].pp =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].accuracy =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].priority =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].target_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].damage_class_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].effect_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].effect_chance =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].contest_type_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].contest_effect_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		moves[i].super_contest_effect_id =  *tmp ? atoi(tmp) : -1;
	}

	fclose(f);

	if (print) {
		for (i = 0; i < 844; i++) {
			printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				   moves[i].id,
				   moves[i].identifier,
				   moves[i].generation_id,
				   moves[i].type_id,
				   moves[i].power,
				   moves[i].pp,
				   moves[i].accuracy,
				   moves[i].priority,
				   moves[i].target_id,
				   moves[i].damage_class_id,
				   moves[i].effect_id,
				   moves[i].effect_chance,
				   moves[i].contest_type_id,
				   moves[i].contest_effect_id,
				   moves[i].super_contest_effect_id);
		}
	}

	prefix = (char *) realloc(prefix, prefix_len + strlen("pokemon_moves.csv") + 1);
	strcpy(prefix + prefix_len, "pokemon_moves.csv");

	f = fopen(prefix, "r");

	//No null byte copied here, so prefix is not technically a string anymore.
	prefix = (char *) realloc(prefix, prefix_len + 1);

	fgets(line, 800, f);

	for (i = 1; i <= 528238; i++) {
		fgets(line, 800, f);
		tmp = next_token(line, ',');
		pokemon_moves[i].pokemon_id = *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		pokemon_moves[i].version_group_id = *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		pokemon_moves[i].move_id = *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		pokemon_moves[i].pokemon_move_method_id = *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		pokemon_moves[i].level = *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		pokemon_moves[i].order = (*tmp != '\n') ? atoi(tmp) : -1;
	}

	fclose(f);

	if (print) {
		for (i = 0; i < 528238; i++) {
			printf("%d %d %d %d %d %d\n",
				   pokemon_moves[i].pokemon_id,
				   pokemon_moves[i].version_group_id,
				   pokemon_moves[i].move_id,
				   pokemon_moves[i].pokemon_move_method_id,
				   pokemon_moves[i].level,
				   pokemon_moves[i].order);
		}
	}

	prefix = (char *) realloc(prefix, prefix_len + strlen("pokemon_species.csv") + 1);
	strcpy(prefix + prefix_len, "pokemon_species.csv");

	f = fopen(prefix, "r");

	//No null byte copied here, so prefix is not technically a string anymore.
	prefix = (char *) realloc(prefix, prefix_len + 1);

	fgets(line, 800, f);

	for (i = 1; i <= 898; i++) {
		fgets(line, 800, f);
		species[i].id = atoi((tmp = next_token(line, ',')));
		strcpy(species[i].identifier, (tmp = next_token(NULL, ',')));
		tmp = next_token(NULL, ',');
		species[i].generation_id = *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].evolves_from_species_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].evolution_chain_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].color_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].shape_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].habitat_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].gender_rate =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].capture_rate =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].base_happiness =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].is_baby =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].hatch_counter =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].has_gender_differences =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].growth_rate_id =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].forms_switchable =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].is_legendary =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].is_mythical =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].order =  *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		species[i].conquest_order =  *tmp ? atoi(tmp) : -1;
	}

	fclose(f);

	if (print) {
		for (i = 0; i <= 898; i++) {
			printf("%d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				   species[i].id,
				   species[i].identifier,
				   species[i].generation_id,
				   species[i].evolves_from_species_id,
				   species[i].evolution_chain_id,
				   species[i].color_id,
				   species[i].shape_id,
				   species[i].habitat_id,
				   species[i].gender_rate,
				   species[i].capture_rate,
				   species[i].base_happiness,
				   species[i].is_baby,
				   species[i].hatch_counter,
				   species[i].has_gender_differences,
				   species[i].growth_rate_id,
				   species[i].forms_switchable,
				   species[i].is_legendary,
				   species[i].is_mythical,
				   species[i].order,
				   species[i].conquest_order);
		}
	}


	prefix = (char *) realloc(prefix, prefix_len + strlen("experience.csv") + 1);
	strcpy(prefix + prefix_len, "experience.csv");

	f = fopen(prefix, "r");

	//No null byte copied here, so prefix is not technically a string anymore.
	prefix = (char *) realloc(prefix, prefix_len + 1);

	fgets(line, 800, f);

	for (i = 1; i <= 600; i++) {
		fgets(line, 800, f);
		experience[i].growth_rate_id = atoi((tmp = next_token(line, ',')));
		tmp = next_token(NULL, ',');
		experience[i].level = *tmp ? atoi(tmp) : -1;
		tmp = next_token(NULL, ',');
		experience[i].experience =  *tmp ? atoi(tmp) : -1;
	}

	fclose(f);

	if (print) {
		for (i = 0; i <= 600; i++) {
			printf("%d %d %d\n",
				   experience[i].growth_rate_id,
				   experience[i].level,
				   experience[i].experience);
		}
	}

	prefix = (char *) realloc(prefix, prefix_len + strlen("type_names.csv") + 1);
	strcpy(prefix + prefix_len, "type_names.csv");

	f = fopen(prefix, "r");

	//No null byte copied here, so prefix is not technically a string anymore.
	prefix = (char *) realloc(prefix, prefix_len + 1);

	fgets(line, 800, f);

	for (i = 1; i <= 18; i++) {
		fgets(line, 800, f); //  1
		fgets(line, 800, f); //  3
		fgets(line, 800, f); //  4
		fgets(line, 800, f); //  5
		fgets(line, 800, f); //  6
		fgets(line, 800, f); //  7
		fgets(line, 800, f); //  8
		fgets(line, 800, f); //  9 - English
		for (j = count = 0; count < 2; j++) {
			if (line[j] == ',') {
				count++;
			}
		}
		line[strlen(line) - 1] = '\0';
		types[i] = strdup(line + j);
		fgets(line, 800, f); // 11
		fgets(line, 800, f); // 12
	}

	fclose(f);

	if (print) {
		for (i = 1; i <= 18; i++) {
			printf("%s\n", types[i]);
		}
	}

	free(prefix);
}

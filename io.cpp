#include "io.h"
#include "pokemon.h"
#include "string.h"

WINDOW *windows[num_windows];
PANEL *panels[num_windows];

world_t world;
void move_char(Character *c, pos_t pos);
void world_changeMap(pos_t to, pos_t from);
char char_getSymbol(character_type_t t);

static void io_initColor() {
	start_color();
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}


void io_init() {
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	io_initColor();
	// Init windows
	windows[win_map] = newwin(MAP_Y, TERMINAL_X, 1, 0);
	windows[win_top] = newwin(1, TERMINAL_X, 0, 0);
	windows[win_bottom] = newwin(2, TERMINAL_X, TERMINAL_Y - 2, 0);
	windows[win_battle] = newwin(MAP_Y, TERMINAL_X, 1, 0);
	windows[win_trainers] = newwin(MAP_Y, TERMINAL_X, 1, 0);
	windows[win_center] = newwin(MAP_Y, TERMINAL_X, 1, 0);
	windows[win_mart] = newwin(MAP_Y, TERMINAL_X, 1, 0);

	windows[win_opponent_summary] = newwin(9, (TERMINAL_X - 4) / 2, 1, 0);
	windows[win_player_summary] = newwin(9, (TERMINAL_X - 4) / 2, 1, (TERMINAL_X) / 2 + 2);
//	windows[win_battle_bag] = newwin(12, 25, 10, TERMINAL_X - 13);
//	windows[win_battle_menu] = newwin(11, 13, TERMINAL_Y-1 - 6, TERMINAL_X - 13);

	windows[win_starters] = newwin(TERMINAL_Y, TERMINAL_X, 0, 0);

	box(windows[win_map], 0, 0);
	box(windows[win_bottom], 0, 0);
	box(windows[win_trainers],0,0);

	// Attach a panel to each window		Order is bottom up
	panels[win_bottom] = new_panel(windows[win_bottom]);
	panels[win_map] = new_panel(windows[win_map]);
	panels[win_top] = new_panel(windows[win_top]);
	panels[win_battle] = new_panel(windows[win_battle]);
	panels[win_trainers] = new_panel(windows[win_trainers]);
	panels[win_player_summary] = new_panel(windows[win_player_summary]);
	panels[win_opponent_summary] = new_panel(windows[win_opponent_summary]);
//	panels[win_battle_bag] = new_panel(windows[win_battle_bag]);
//	panels[win_battle_menu] = new_panel(windows[win_battle_menu]);
	panels[win_starters] = new_panel(windows[win_starters]);
	panels[win_center] = new_panel(windows[win_center]);
	panels[win_mart] = new_panel(windows[win_mart]);

	hide_panel(panels[win_trainers]);
	hide_panel(panels[win_battle]);
	hide_panel(panels[win_player_summary]);
	hide_panel(panels[win_opponent_summary]);
	hide_panel(panels[win_starters]);
	hide_panel(panels[win_center]);
	hide_panel(panels[win_mart]);
//	hide_panel(panels[win_battle_bag]);
//	hide_panel(panels[win_battle_menu]);

	update_panels(); // Write panels to vitual screen in correct visibility order
	doupdate(); // Shows to screen
}

void io_end(){
	endwin();
//	while(io_head) {
//		io_tail = io_head;
//		io_head = io_head->next;
//		free(io_tail);
//	}
//	io_tail = NULL;
}

/*
 * show pokemon stats on window with top left origin pos
 */
static void io_show_pokemon_summary(Pokemon* pk, WINDOW* win, Pos pos) {
	mvwprintw(win, pos.y, pos.x, "%s(%c) lvl: %d", pk->get_species(), pk->get_gender_string()[0], pk->get_level());
	mvwprintw(win, pos.y+1, pos.x, "%s %s", pk->get_type(0), pk->get_type(1));
	mvwprintw(win, pos.y+2, pos.x, "Moves:");
	mvwprintw(win, pos.y+3, pos.x, "  %s", pk->get_move(0));
	mvwprintw(win, pos.y+4, pos.x, "  %s", pk->get_move(1));
	mvwprintw(win, pos.y+5, pos.x, "  %s", pk->get_move(2));
	mvwprintw(win, pos.y+6, pos.x, "  %s", pk->get_move(3));
	char* type_line;
}

void io_get_starter() {
	int key;
	int reroll = 1;
	Pokemon* p1;
	Pokemon* p2;
	Pokemon* p3;
	while (reroll) {
		show_panel(panels[win_starters]);
		top_panel(panels[win_starters]);
		wclear(windows[win_starters]);
		mvwaddstr(windows[win_starters], TERMINAL_Y - 1, 0,
				  "Select your starter using 1, 2, or 3. Press r to roll again");

		// Make pokemon
		p1 = new Pokemon(1);
		p2 = new Pokemon(1);
		p3 = new Pokemon(1);

		// Add to display
		io_show_pokemon_summary(p1, windows[win_starters], Pos{1, 1});
		io_show_pokemon_summary(p2, windows[win_starters], Pos{26, 1});
		io_show_pokemon_summary(p3, windows[win_starters], Pos{55, 1});
		update_panels();
		doupdate();
		int valid = 0;
		while (!valid) {
			flushinp();
			key = getch();
			switch (key) {
				case '1':

					delete p2;
					delete p3;
					world.pc->addPokemon(p1);
					reroll = 0;
					valid = 1;
					break;
				case '2':
					delete p1;
					delete p3;
					world.pc->addPokemon(p2);
					reroll = 0;
					valid = 1;
					break;
				case '3':
					delete p1;
					delete p2;
					world.pc->addPokemon(p3);
					reroll = 0;
					valid = 1;
					break;
				case 'r':
					delete p1;
					delete p2;
					delete p3;
					reroll = 1;
					valid = 1;
					break;
				default:
					valid = 0;
					break;
			}
		}
	}
	hide_panel(panels[win_starters]);
}

void io_display_map() {
	wclear(windows[win_map]);
	int x, y;
	for (y = 0; y < MAP_Y; y++) {
		for(x = 0; x < MAP_X; x++) {
			if (w_charxy(x,y) != NULL) {
				wattron(windows[win_map], COLOR_PAIR(COLOR_WHITE));
				mvwaddch(windows[win_map], y, x, w_charxy(x,y)->getSymbol());
				wattroff(windows[win_map], COLOR_PAIR(COLOR_WHITE));
			} else {
				switch (w_terxy(x,y)) {
					case ter_boulder:
					case ter_mountain:
					case ter_border:
						wattron(windows[win_map], COLOR_PAIR(COLOR_MAGENTA));
						mvwaddch(windows[win_map],y, x, ter_getSymbol(ter_boulder));
						wattroff(windows[win_map], COLOR_PAIR(COLOR_MAGENTA));
						break;
					case ter_tree:
					case ter_forest:
						wattron(windows[win_map], COLOR_PAIR(COLOR_GREEN));
						mvwaddch(windows[win_map],y, x, ter_getSymbol(ter_tree));
						wattroff(windows[win_map], COLOR_PAIR(COLOR_GREEN));
						break;
					case ter_path:
					case ter_exit:
						wattron(windows[win_map], COLOR_PAIR(COLOR_YELLOW));
						mvwaddch(windows[win_map],y, x, ter_getSymbol(ter_path));
						wattroff(windows[win_map], COLOR_PAIR(COLOR_YELLOW));
						break;
					case ter_mart:
						wattron(windows[win_map], COLOR_PAIR(COLOR_BLUE));
						mvwaddch(windows[win_map],y, x, ter_getSymbol(ter_mart));
						wattroff(windows[win_map], COLOR_PAIR(COLOR_BLUE));
						break;
					case ter_center:
						wattron(windows[win_map], COLOR_PAIR(COLOR_RED));
						mvwaddch(windows[win_map],y, x, ter_getSymbol(ter_center));
						wattroff(windows[win_map], COLOR_PAIR(COLOR_RED));
						break;
					case ter_grass:
						wattron(windows[win_map], COLOR_PAIR(COLOR_GREEN));
						mvwaddch(windows[win_map],y, x, ter_getSymbol(ter_grass));
						wattroff(windows[win_map], COLOR_PAIR(COLOR_GREEN));
						break;
					case ter_clearing:
						wattron(windows[win_map], COLOR_PAIR(COLOR_GREEN));
						mvwaddch(windows[win_map],y, x, ter_getSymbol(ter_clearing));
						wattroff(windows[win_map], COLOR_PAIR(COLOR_GREEN));
						break;
					case ter_water:
						wattron(windows[win_map], COLOR_PAIR(COLOR_CYAN));
						mvwaddch(windows[win_map],y,x, ter_getSymbol(ter_water));
						wattroff(windows[win_map], COLOR_PAIR(COLOR_CYAN));
						break;

					default:
						wattron(windows[win_map], COLOR_PAIR(COLOR_CYAN));
						mvwaddch(windows[win_map],y, x, '0');
						wattroff(windows[win_map], COLOR_PAIR(COLOR_CYAN));
				}
			}
		}
	}
	update_panels();
	doupdate();
}

void io_display_message(const char* str, bool acknowledge) {
	wclear(windows[win_top]);
	mvwaddstr(windows[win_top], 0, 0,str);
	update_panels();
	doupdate();
	if(acknowledge) {
		flushinp();
		getch();
	}
}

void io_clear_message() {
	wclear(windows[win_top]);
	update_panels();
	doupdate();
}

static int compare_trainer_distance(const void *v1, const void *v2)
{
	const Character *const *c1 = (const Character *const *) v1;
	const Character *const *c2 = (const Character *const *) v2;

	return (world.rival_dist[(*c1)->pos.y][(*c1)->pos.x] -
			world.rival_dist[(*c2)->pos.y][(*c2)->pos.x]);
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
	uint32_t offset;
	uint32_t i;

	offset = 0;

	while (1) {
		for (i = 0; i < 13; i++) {
			mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
			update_panels();
			doupdate();
		}
		switch (getch()) {
			case KEY_UP:
				if (offset) {
					offset--;
				}
				break;
			case KEY_DOWN:
				if (offset < (count - 13)) {
					offset++;
				}
				break;
			case 27:
				return;
		}

	}
}

static void io_list_trainers_display(Npc **c,
									 uint32_t count)
{
	uint32_t i;
	char (*s)[40]; /* pointer to array of 40 char */

	s = (char (*)[40]) malloc(count * sizeof (*s));


	show_panel(panels[win_trainers]);
	top_panel(panels[win_trainers]);
	wclear(windows[win_trainers]);

	mvprintw(3, 19, " %-40s ", "");
	/* Borrow the first element of our array for this string: */
	snprintf(s[0], 40, "You know of %d trainers:", count);
	mvprintw(4, 19, " %-40s ", s[0]);
	mvprintw(5, 19, " %-40s ", "");

	for (i = 0; i < count; i++) {
		snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
				 (*c)->getName(),
				 (*c)->getSymbol(),
				 abs(c[i]->pos.y - world.pc->pos.y),
				 ((c[i]->pos.y - world.pc->pos.y) <= 0 ?
				  "North" : "South"),
				 abs(c[i]->pos.x - world.pc->pos.x),
				 ((c[i]->pos.x - world.pc->pos.x) <= 0 ?
				  "West" : "East"));
		if (count <= 13) {
			/* Handle the non-scrolling case right here. *
			 * Scrolling in another function.            */
			mvprintw(i + 6, 19, " %-40s ", s[i]);
		}
	}

	if (count <= 13) {
		mvprintw(count + 6, 19, " %-40s ", "");
		mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
		while (getch() != 27 /* escape */)
			;
	} else {
		mvprintw(19, 19, " %-40s ", "");
		mvprintw(20, 19, " %-40s ",
				 "Arrows to scroll, escape to continue.");
		io_scroll_trainer_list(s, count);
	}

	free(s);
	hide_panel(panels[win_trainers]);
	top_panel(panels[win_map]);
}

static void io_list_trainers()
{
	Character **c;
	uint32_t x, y, count;

	c = (Character **) malloc(num_trainers * sizeof (*c));

	/* Get a linear list of trainers */
	for (count = 0, y = 1; y < MAP_Y - 1; y++) {
		for (x = 1; x < MAP_X - 1; x++) {
			if (w_charxy(x,y) && w_charxy(x,y) != (Character*)&world.pc) {
				c[count++] = w_charxy(x,y);
			}
		}
	}

	/* Sort it by distance from PC */
	qsort(c, count, sizeof (*c), compare_trainer_distance);

	/* Display it */
	io_list_trainers_display((Npc **)(c), count);
	free(c);

}

static void io_display_battle() {
	wclear(windows[win_battle]);
	int i;

	// Swap pokemon
	int party_x = 1;
	int party_y = TERMINAL_Y-10;
	mvwprintw(windows[win_battle], party_y, party_x, "Swap Pokemon:");
	for (i = 0; i < world.pc->pokemon.size() && i < 6; i++) {
		mvwprintw(windows[win_battle], party_y+1+i, party_x, "%c) %.10s Lv:%d HP:%d/%d",
				  'a' + i,
				  world.pc->pokemon.at(i)->get_species(),
				  world.pc->pokemon.at(i)->get_level(),
				  world.pc->pokemon.at(i)->get_hp(),
				  world.pc->pokemon.at(i)->get_max_hp());
	}

	// Bag Items 30
	int bag_x = party_x + 31;
	int bag_y = TERMINAL_Y-10;
	mvwprintw(windows[win_battle], bag_y, bag_x, "Bag Items:");
	mvwprintw(windows[win_battle], bag_y+1, bag_x, "x - pokeball (x%d)", world.pc->pokeballs);
	mvwprintw(windows[win_battle], bag_y+2, bag_x, "y - potion   (x%d)", world.pc->potions);
	mvwprintw(windows[win_battle], bag_y+3, bag_x, "z - revive   (x%d)", world.pc->revives);

	// Run
	mvwprintw(windows[win_player_summary], 7, 1, "5) run");


	update_panels();
	doupdate();
}

static void io_display_pokemon_summary(window_type_t win, Pokemon* pk) {
	show_panel(panels[win]);
	top_panel(panels[win]);
	wclear(windows[win]);
	box(windows[win],0,0);
	if (win == win_opponent_summary) {
		mvwprintw(windows[win], 0, 1, "Opponent:");
	}
	else if (win == win_player_summary) {
		mvwprintw(windows[win], 0, 1, "You:");
	}
	mvwprintw(windows[win], 1, 1, "%s Lv: %d", pk->get_species(), pk->get_level());
	mvwprintw(windows[win], 2, 1, "(%c) %s", pk->get_gender_string()[0], pk->get_shiny_string());
	mvwprintw(windows[win], 3, 1, "[%s %s]", pk->get_type(0), pk->get_type(1));
	mvwprintw(windows[win], 4, 1, "HP: %d / %d", pk->get_hp(), pk->get_max_hp());
	mvwprintw(windows[win], 5, 1, "Moves:");
	char moveset[TERMINAL_X / 2];
	sprintf(moveset, "");
	if (strcmp(pk->get_move(0), "") != 0) {
		strcat(moveset, "1) ");
		strcat(moveset, pk->get_move(0));
	}
	if (strcmp(pk->get_move(1), "") != 0) {
		strcat(moveset, "  2) ");
		strcat(moveset, pk->get_move(1));
	}
	mvwprintw(windows[win], 5, 1, "%s", moveset);

	sprintf(moveset, "");
	if (strcmp(pk->get_move(2), "") != 0) {
		strcat(moveset, "3) ");
		strcat(moveset, pk->get_move(2));
	}
	if (strcmp(pk->get_move(3), "") != 0) {
		strcat(moveset, "  4) ");
		strcat(moveset, pk->get_move(0));
	}
	mvwprintw(windows[win], 6, 1, "%s", moveset);
	update_panels();
	doupdate();
}


int rand_level() {// Calculate level
	int min_level, max_level, level;
	int dist = manhattan(world.cur_idx, (Pos){((WORLD_X - 1) / 2), ((WORLD_Y - 1) / 2)});
	if (dist <= 200) {
		min_level = 1;
		max_level = dist / 2;
	} else {
		min_level = (dist - 200) / 2;
		max_level = 100;
	}
	if (max_level < 1) {
		max_level = 1;
	}
	return rand() % (max_level - min_level + 1) + min_level;
}

static int escape(Pokemon* pc_pokemon, Pokemon* wild_pokemon, int attempts) {
	float fraction = (pc_pokemon->get_speed() * 32) / ((wild_pokemon->get_speed() / 4) % 256);
	float odds = fraction + (30 * attempts);
	return (rand() % 256) < odds ? 1 : 0;
}

static float getTypeEffectiveness(const Pokemon *aggressor, int move_slot) {
	float type_eff;

	int i;
	for (i = 0; i < 325; i++) {
		if (type_effectiveness[i].damage_type_id == moves[aggressor->move_index[move_slot]].type_id &&
			type_effectiveness[i].target_type_id == aggressor->type_index[0]) {
			type_eff = type_effectiveness[i].damage_factor;
		}
	}

	if (aggressor->type_index.size() > 0) {
		for (i = 0; i < 325; i++) {
			if (type_effectiveness[i].damage_type_id == moves[aggressor->move_index[move_slot]].type_id &&
				type_effectiveness[i].target_type_id == aggressor->type_index[0]) {
				type_eff *= type_effectiveness[i].damage_factor;
			}
		}
	}
	return type_eff;
}

static int calculate_damage(Pokemon* aggressor, Pokemon* defender, int move_slot) {
	float critical = 1;
	if (rand() % 256 < aggressor->base_speed/2) {
		critical = 1.5;
	}

	float random = ((rand() % 16) + 85) / 100.0;

	float stab = 1;
	if (((aggressor->type_index.size() > 0) && (moves[aggressor->move_index[move_slot]].type_id == aggressor->type_index[0])) ||
		((aggressor->type_index.size() > 1) && (moves[aggressor->move_index[move_slot]].type_id == aggressor->type_index[1]))) {
		stab = 1.5;
	}

	float type_eff = getTypeEffectiveness(aggressor, move_slot);


	float damage = (((2 * aggressor->get_level()) / 5.0) + 2);
	damage = damage * moves[aggressor->move_index[move_slot]].power * ((aggressor->get_atk() * 1.0)/defender->get_def());
	damage = (damage / 50.0) + 2;
	damage = damage * critical;
	damage = damage * random;
	damage = damage * stab;
	damage = damage * type_eff;
	if (damage < 0) {
		damage = 0;
	}
	return (int)damage;
}

static int heal_pokemon(Pokemon *curr_pokemon, int party_index) {
	int done;
	char msg[TERMINAL_X];
	if (world.pc->pokemon.size() < party_index+1) {
		io_display_message("You do not have a pokemon in that slot...", true);
		done = 0;
	} else if (world.pc->pokemon[party_index]->get_hp() == world.pc->pokemon[party_index]->get_max_hp()) {
		io_display_message("This pokemon is already healthy...", true);
		done = 0;
	} else if(world.pc->pokemon[party_index]->get_hp() == 0) {
		io_display_message("You cant use a potion on a defeated pokemon. Use a revive...", true);
		done = 0;
	} else {
		world.pc->pokemon[party_index]->set_hp(world.pc->pokemon[party_index]->get_hp() + 20);
		world.pc->potions--;
		if (curr_pokemon != nullptr) {
			io_display_pokemon_summary(win_player_summary, curr_pokemon);
		}
		io_display_battle();
		sprintf(msg, "You healed %s...", world.pc->pokemon[party_index]->get_species());
		io_display_message(msg, true);
		done = 1;
	}
	return done;
}

static int revive_pokemon(Pokemon *curr_pokemon, int party_index) {
	int done;
	char msg[TERMINAL_X];
	if (world.pc->pokemon.size() < party_index+1) {
		io_display_message("You do not have a pokemon in that slot...", true);
		done = 0;
	} else if (world.pc->pokemon[party_index]->get_hp() != 0) {
		io_display_message("This pokemon isn't defeated...", true);
		done = 0;
	} else {
		world.pc->pokemon[party_index]->set_hp(world.pc->pokemon[party_index]->get_max_hp() / 2);
		world.pc->revives--;
		if (curr_pokemon != nullptr) {
			io_display_pokemon_summary(win_player_summary, curr_pokemon);
		}
		io_display_battle();
		sprintf(msg, "You revived %s...", world.pc->pokemon[party_index]->get_species());
		io_display_message(msg, true);
		done = 1;
	}
	return done;
}

static void io_battle_revive(Pokemon* curr_pokemon, int &turn_consumed) {
	int done;
	int key;
	char msg[TERMINAL_X];
	if (world.pc->revives <=0) {
		world.pc->revives = 0;
		return;
	}
	done = 0;
	while (!done) {
		io_display_message("Select the pokemon to revive (+50% HP from 0), or press ESC to back out.", false);
		flushinp();
		key = getch();
		switch (key) {
			case 'a': // Swap to pokemon 1
				done = revive_pokemon(curr_pokemon, 0);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'b': // Swap to pokemon 2
				done = revive_pokemon(curr_pokemon, 1);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'c': // Swap to pokemon 3
				done = revive_pokemon(curr_pokemon, 2);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'd': // Swap to pokemon 4
				done = revive_pokemon(curr_pokemon, 3);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'e': // Swap to pokemon 5
				done = revive_pokemon(curr_pokemon, 4);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'f': // Swap to pokemon 6
				done = revive_pokemon(curr_pokemon, 5);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 27:
				done = 1;
				turn_consumed = 0;
				break;
			default:
				break;
		}
	}
}

static void io_battle_potion(Pokemon* curr_pokemon, int &turn_consumed) {
	int done;
	int key;
	char msg[TERMINAL_X];

	if (world.pc->potions <=0) {
		world.pc->potions = 0;
		return;
	}

	done = 0;
	while (!done) {
		io_display_message("Select the pokemon to heal (+20HP), or press ESC to back out.", false);
		flushinp();
		key = getch();
		switch (key) {
			case 'a': // Swap to pokemon 1
				done = heal_pokemon(curr_pokemon, 0);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'b': // Swap to pokemon 2
				done = heal_pokemon(curr_pokemon, 1);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'c': // Swap to pokemon 3
				done = heal_pokemon(curr_pokemon, 2);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'd': // Swap to pokemon 4
				done = heal_pokemon(curr_pokemon, 3);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'e': // Swap to pokemon 5
				done = heal_pokemon(curr_pokemon, 4);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 'f': // Swap to pokemon 6
				done = heal_pokemon(curr_pokemon, 5);
				if (done) {
					turn_consumed = 1;
				}
				break;
			case 27:
				done = 1;
				turn_consumed = 0;
				break;
			default:
				break;
		}
	}
}

static void swap_pokemon(const Pc *pc, Pokemon *&curr_pokemon, int &turn_consumed, int party_index) {
	if (pc->pokemon.size() < party_index+1) {
		io_display_message("You do not have a pokemon in that slot...", true);
	} else if (curr_pokemon == pc->pokemon[party_index]) {
		io_display_message("This pokemon is already in battle...", true);
	} else if (pc->pokemon[party_index]->get_hp() == 0){
		io_display_message("You cant swap a defeated pokemon into battle...", true);
	}else {
		curr_pokemon = pc->pokemon[party_index];
		io_display_pokemon_summary(win_player_summary, curr_pokemon);
		io_display_message("You swapped pokemon...", true);
		turn_consumed = 1;
	}
}

static void io_bag() {
	int done;
	int key;
	show_panel(panels[win_battle]);
	top_panel(panels[win_battle]);
	done = 0;
	while(!done) {
		wclear(windows[win_battle]);
		io_display_message("You opened your bag. Choose an item to use, or press ESC to back out", false);
		io_display_battle();

		flushinp();
		key = getch();
		switch (key) {
			case 'x':
				io_display_message("You can't use pokeballs outside of battle...", true);
				done = 0;
				break;
			case 'y':
				// Use potion
				io_battle_potion(nullptr, done);
				done = 0;
				break;
			case 'z':
				// Use revive
				io_battle_revive(nullptr, done);
				done = 0;
				break;
			case 27: // quit
				done = 1;
				break;
			default:
				done = 0;
				break;
		}
	}
	wclear(windows[win_battle]);
	wclear(windows[win_top]);
	hide_panel(panels[win_battle]);

}

static void io_random_encounter() {
	Pokemon *wild_pokemon = new Pokemon(rand_level());
	Pokemon *curr_pokemon = nullptr;
	Pc *pc = world.pc;
	int battle_over;
	int turn_consumed;
	int opp_move;
	move_t pc_move;
	int ran;
	int escape_attempts;
	int key;
	int i;
	show_panel(panels[win_battle]);
	top_panel(panels[win_battle]);
	// Send out first available pokemon
	curr_pokemon = pc->firstAvailablePokemon();
	if (curr_pokemon == nullptr) {
		hide_panel(panels[win_opponent_summary]);
		hide_panel(panels[win_player_summary]);
		hide_panel(panels[win_battle]);
		wclear(windows[win_battle]);
		wclear(windows[win_top]);
		return;
	}

	io_display_message("A wild pokemon appeared!", true);
	battle_over = 0;
	turn_consumed = 0;
	escape_attempts = 0;
	ran = 0;
	while (!battle_over) {
		while (!turn_consumed && !battle_over) {
			io_display_pokemon_summary(win_player_summary, curr_pokemon);
			io_display_pokemon_summary(win_opponent_summary, wild_pokemon);
			io_display_battle();
			io_display_message("Select your action.", false);
			flushinp();
			key = getch();
			switch (key) {
				case '1':
					if (curr_pokemon->move_index[0] == 0) {
						// Not valid
						io_display_message("Your pokemon does not have a move in slot 1...", true);
					} else {
						pc_move = move_1;
						turn_consumed = 1;
					}
					break;
				case '2':
					if (curr_pokemon->move_index[1] == 0) {
						// Not valid
						io_display_message("Your pokemon does not have a move in slot 2...", true);
					} else {
						pc_move = move_2;
						turn_consumed = 1;
					}
					break;
				case '3':
					if (curr_pokemon->move_index[2] == 0) {
						// Not valid
						io_display_message("Your pokemon does not have a move in slot 3...", true);
					} else {
						pc_move = move_3;
						turn_consumed = 1;
					}
					break;
				case '4':
					if (curr_pokemon->move_index[3] == 0) {
						// Not valid
						io_display_message("Your pokemon does not have a move in slot 4...", true);
					} else {
						pc_move = move_4;
						turn_consumed = 1;
					}
					break;
				case '5': // Run
					if (escape(curr_pokemon, wild_pokemon, escape_attempts)) {
						io_display_message("You ran away successfully...", true);
						battle_over = 1;
					} else {
						io_display_message("You could not run away...", true);
						escape_attempts++;
						pc_move = nop;
					}
					turn_consumed = 1;
					break;
				case 'a': // Swap to pokemon 1
					swap_pokemon(pc, curr_pokemon, turn_consumed, 0);
					pc_move = nop;
					break;
				case 'b': // Swap to pokemon 2
					swap_pokemon(pc, curr_pokemon, turn_consumed, 1);
					pc_move = nop;
					break;
				case 'c': // Swap to pokemon 3
					swap_pokemon(pc, curr_pokemon, turn_consumed, 2);
					pc_move = nop;
					break;
				case 'd': // Swap to pokemon 4
					swap_pokemon(pc, curr_pokemon, turn_consumed, 3);
					pc_move = nop;
					break;
				case 'e': // Swap to pokemon 5
					swap_pokemon(pc, curr_pokemon, turn_consumed, 4);
					pc_move = nop;
					break;
				case 'f': // Swap to pokemon 6
					swap_pokemon(pc, curr_pokemon, turn_consumed, 5);
					pc_move = nop;
					break;
				case 'x': // Attempt catch
				{
					if (pc->pokeballs <= 0) {
						io_display_message("You don't have any pokeballs...", true);
						break;
					}
					pc->pokeballs--;

					int m = rand() % 256;
					int f = (int)((wild_pokemon->get_max_hp() * 255 * 4.0) / (wild_pokemon->get_hp() * 12.0));
					if (f < 1) {
						f = 1;
					} else if (f > 255) {
						f = 255;
					}

					if (f >= m) {
						// Caught

						if (pc->pokemon.size() < 6) {
							io_display_message("The wild pokemon has been added to your team...", true);
							pc->addPokemon(wild_pokemon);
							battle_over = 1;
						} else {
							io_display_message("The wild pokemon has been added to your box...", true);
							pc->box.push_back(wild_pokemon);
							battle_over = 1;
						}
					} else {
						io_display_message("The wild pokemon broke free from the pokeball...", true);
						battle_over = 0;
					}
					break;
				}
				case 'y':
					// Use potion
					if (pc->potions <= 0) {
						io_display_message("You don't have any potions...", true);
						break;
					}
					io_battle_potion(curr_pokemon, turn_consumed);
					pc_move = nop;
					break;
				case 'z':
					// Use revive
					if (pc->revives <= 0) {
						io_display_message("You don't have any revives...", true);
						break;
					}
					io_battle_revive(curr_pokemon, turn_consumed);
					pc_move = nop;
					break;
				case 'Q': // quit game
					exit(0);
					break;
				default:
					break;
			} // switch(key)
		} // while turn not consumed
		turn_consumed = 0;

		if (battle_over == 1) {
			break;
		}

		// Select action for opponent
		opp_move = (move_t)(rand() % wild_pokemon->get_num_moves());

		// Determine who goes first
		int pc_first = 0;
		{
			if (pc_move == nop) {
				// Wild pokemon move
				pc_first = 0;
			} else {

				if (moves[curr_pokemon->move_index[pc_move]].priority >
					moves[wild_pokemon->move_index[opp_move]].priority) {
					// PC First
					pc_first = 1;
				} else if (moves[curr_pokemon->move_index[pc_move]].priority <
						   moves[wild_pokemon->move_index[opp_move]].priority) {
					// Wild pokemon first
					pc_first = 0;
				} else {
					int sp = curr_pokemon->get_speed() - wild_pokemon->get_speed();
					if (sp < 0) {
						// Wild pokemon first
						pc_first = 0;
					} else {
						// pc first
						pc_first = 1;
					}
				}

			}
		}

		// Calculate turn
		int dmg;
		char msg[TERMINAL_X];
		if (pc_first) {
			if (rand() % 100 < moves[curr_pokemon->move_index[pc_move]].accuracy) {
				// hit
				dmg = calculate_damage(curr_pokemon, wild_pokemon, pc_move);
				wild_pokemon->take_damage(dmg);
				io_display_pokemon_summary(win_opponent_summary, wild_pokemon);
				sprintf(msg, "%s did %d damage to the wild %s...", curr_pokemon->get_species(), dmg, wild_pokemon->get_species());
				io_display_message(msg, true);
			} else {
				// miss
				sprintf(msg, "%s missed its attack...", curr_pokemon->get_species());
				io_display_message(msg, true);
			}

			// Wild dead
			if (wild_pokemon->get_hp() == 0) {
				sprintf(msg, "You defeated the wild %s...", wild_pokemon->get_species());
				io_display_message(msg, true);
				battle_over = 1;
			}
			// Wild turn
			else {
				if (rand() % 100 < moves[wild_pokemon->move_index[opp_move]].accuracy) {
					dmg = calculate_damage(wild_pokemon, curr_pokemon, opp_move);
					curr_pokemon->take_damage(dmg);
					io_display_pokemon_summary(win_player_summary, curr_pokemon);
					sprintf(msg, "The wild %s did %d damage to your %s...", wild_pokemon->get_species(), dmg, curr_pokemon->get_species());
					io_display_message(msg, true);
				} else {
					// miss
					sprintf(msg, "%s missed its attack...", wild_pokemon->get_species());
					io_display_message(msg, true);
				}

				// Curr dead
				if (curr_pokemon->get_hp() == 0) {
					sprintf(msg, "The wild %s defeated your %s...", wild_pokemon->get_species(), curr_pokemon->get_species());
					io_display_message(msg, true);

					// Check if any more valid pokemon
					if (pc->firstAvailablePokemon() == nullptr) {
						io_display_message("You have no more undefeated pokemon...", true);
						battle_over = 1;
					} else {
						curr_pokemon = pc->firstAvailablePokemon();
						io_display_pokemon_summary(win_player_summary, curr_pokemon);
						sprintf(msg, "%s has been subbed in...", curr_pokemon->get_species());
						io_display_message(msg, true);
					}
				}
			}
		}

		else { // Wild pokemon first
			if (rand() % 100 < moves[wild_pokemon->move_index[opp_move]].accuracy) {
				dmg = calculate_damage(wild_pokemon, curr_pokemon, opp_move);
				curr_pokemon->take_damage(dmg);
				io_display_pokemon_summary(win_player_summary, curr_pokemon);
				sprintf(msg, "The wild %s did %d damage to your %s...", wild_pokemon->get_species(), dmg,
						curr_pokemon->get_species());
				io_display_message(msg, true);
			} else {
				// miss
				sprintf(msg, "%s missed its attack...", wild_pokemon->get_species());
				io_display_message(msg, true);
			}
			// Curr dead
			if (curr_pokemon->get_hp() == 0) {
				sprintf(msg, "The wild %s defeated your %s...", wild_pokemon->get_species(), curr_pokemon->get_species());
				io_display_message(msg, true);

				// Check if any more valid pokemon
				if (pc->firstAvailablePokemon() == nullptr) {
					io_display_message("You have no more undefeated pokemon...", true);
					battle_over = 1;
				} else {
					curr_pokemon = pc->firstAvailablePokemon();
					io_display_pokemon_summary(win_player_summary, curr_pokemon);
					sprintf(msg, "%s has been subbed in...", curr_pokemon->get_species());
					io_display_message(msg, true);
				}
			}

			// Pc pokemon turn
			else {
				if (pc_move != nop) {
					if (rand() % 100 < moves[curr_pokemon->move_index[pc_move]].accuracy) {
						dmg = calculate_damage(curr_pokemon, wild_pokemon, pc_move);
						wild_pokemon->take_damage(dmg);
						io_display_pokemon_summary(win_opponent_summary, wild_pokemon);
						sprintf(msg, "%s did %d damage to the wild %s...", curr_pokemon->get_species(), dmg,
								wild_pokemon->get_species());
						io_display_message(msg, true);
					} else {
						// miss
						sprintf(msg, "%s missed its attack...", curr_pokemon->get_species());
						io_display_message(msg, true);
					}

					// Wild dead
					if (wild_pokemon->get_hp() == 0) {
						sprintf(msg, "You defeated the wild %s...", wild_pokemon->get_species());
						io_display_message(msg, true);
						battle_over = 1;
					}
				}
			}
		}
	} // while battle not over
	hide_panel(panels[win_opponent_summary]);
	hide_panel(panels[win_player_summary]);
	hide_panel(panels[win_battle]);
	wclear(windows[win_battle]);
	wclear(windows[win_top]);

}

void io_trainer_battle(Npc *c) {
//	// Battle
//	int done;
//	int key;
//	done  = 0;
//	// Right now, display to panel
//	show_panel(panels[win_battle]);
//	wclear(windows[win_battle]);
//	while (!done) {
//		box(windows[win_battle],0,0);
//		mvwaddstr(windows[win_battle], 4,4,"BATTLE: Press ESC to defeat your opponent!");
//
//		update_panels();
//		doupdate();
//		flushinp();
//		key = getch();
//		switch (key) {
//			case 27: // ESC
//				c->defeated = 1;
//				wclear(windows[win_battle]);
//				hide_panel(panels[win_battle]);
//				// PC wins
//				// Mark trainer as defeated
//				// Return to map
//				done = 1;
//				break;
//		}
//	}
	Pokemon *npc_pokemon = new Pokemon(rand_level());
	Pokemon *curr_pokemon = nullptr;
	Pc *pc = world.pc;
	char msg[TERMINAL_X];
	int battle_over;
	int turn_consumed;
	int npc_move;
	move_t pc_move;
	int key;
	int i;
	show_panel(panels[win_battle]);
	top_panel(panels[win_battle]);
	// Send out first available pokemon
	curr_pokemon = pc->firstAvailablePokemon();
	if (curr_pokemon == nullptr) {
		hide_panel(panels[win_opponent_summary]);
		hide_panel(panels[win_player_summary]);
		hide_panel(panels[win_battle]);
		wclear(windows[win_battle]);
		wclear(windows[win_top]);
		return;
	}

	sprintf(msg, "%s challenged you to a pokemon battle!", c->getName());
	io_display_message(msg, true);
	battle_over = 0;
	turn_consumed = 0;
	while (!battle_over) {
		while (!turn_consumed && !battle_over) {
			io_display_pokemon_summary(win_player_summary, curr_pokemon);
			io_display_pokemon_summary(win_opponent_summary, npc_pokemon);
			io_display_battle();
			io_display_message("Select your action.", false);
			flushinp();
			key = getch();
			switch (key) {
				case '1':
					if (curr_pokemon->move_index[0] == 0) {
						// Not valid
						io_display_message("Your pokemon does not have a move in slot 1...", true);
					} else {
						pc_move = move_1;
						turn_consumed = 1;
					}
					break;
				case '2':
					if (curr_pokemon->move_index[1] == 0) {
						// Not valid
						io_display_message("Your pokemon does not have a move in slot 2...", true);
					} else {
						pc_move = move_2;
						turn_consumed = 1;
					}
					break;
				case '3':
					if (curr_pokemon->move_index[2] == 0) {
						// Not valid
						io_display_message("Your pokemon does not have a move in slot 3...", true);
					} else {
						pc_move = move_3;
						turn_consumed = 1;
					}
					break;
				case '4':
					if (curr_pokemon->move_index[3] == 0) {
						// Not valid
						io_display_message("Your pokemon does not have a move in slot 4...", true);
					} else {
						pc_move = move_4;
						turn_consumed = 1;
					}
					break;
				case '5':
					io_display_message("You cant run away from a trainer...", true);
					turn_consumed = 0;
					break;
				case 'a': // Swap to pokemon 1
					swap_pokemon(pc, curr_pokemon, turn_consumed, 0);
					pc_move = nop;
					break;
				case 'b': // Swap to pokemon 2
					swap_pokemon(pc, curr_pokemon, turn_consumed, 1);
					pc_move = nop;
					break;
				case 'c': // Swap to pokemon 3
					swap_pokemon(pc, curr_pokemon, turn_consumed, 2);
					pc_move = nop;
					break;
				case 'd': // Swap to pokemon 4
					swap_pokemon(pc, curr_pokemon, turn_consumed, 3);
					pc_move = nop;
					break;
				case 'e': // Swap to pokemon 5
					swap_pokemon(pc, curr_pokemon, turn_consumed, 4);
					pc_move = nop;
					break;
				case 'f': // Swap to pokemon 6
					swap_pokemon(pc, curr_pokemon, turn_consumed, 5);
					pc_move = nop;
					break;
				case 'x': // Attempt catch
					io_display_message("You cant use that here...", true);
					turn_consumed = 0;
					break;
				case 'y':
					// Use potion
					io_battle_potion(curr_pokemon, turn_consumed);
					pc_move = nop;
					break;
				case 'z':
					// Use revive
					io_battle_revive(curr_pokemon, turn_consumed);
					pc_move = nop;
					break;
				case 'Q': // quit game
					exit(0);
					break;
				default:
					break;
			} // switch(key)
		} // while turn not consumed
		turn_consumed = 0;

//		if (battle_over == 1) {
//			break;
//		}

		// Select action for opponent
		npc_move = (move_t)(rand() % npc_pokemon->get_num_moves());

		// Determine who goes first
		int pc_first = 0;
		{
			if (pc_move == nop) {
				// NPC pokemon move
				pc_first = 0;
			} else {

				if (moves[curr_pokemon->move_index[pc_move]].priority >
					moves[npc_pokemon->move_index[npc_move]].priority) {
					// PC First
					pc_first = 1;
				} else if (moves[curr_pokemon->move_index[pc_move]].priority <
						   moves[npc_pokemon->move_index[npc_move]].priority) {
					// NPC pokemon first
					pc_first = 0;
				} else {
					int sp = curr_pokemon->get_speed() - npc_pokemon->get_speed();
					if (sp < 0) {
						// NPC pokemon first
						pc_first = 0;
					} else {
						// pc first
						pc_first = 1;
					}
				}

			}
		}

		// Calculate turn
		int dmg;
		if (pc_first) {
			if (rand() % 100 < moves[curr_pokemon->move_index[pc_move]].accuracy) {
				// hit
				dmg = calculate_damage(curr_pokemon, npc_pokemon, pc_move);
				npc_pokemon->take_damage(dmg);
				io_display_pokemon_summary(win_opponent_summary, npc_pokemon);
				sprintf(msg, "%s did %d damage to the the trainer's %s...", curr_pokemon->get_species(), dmg, npc_pokemon->get_species());
				io_display_message(msg, true);
			} else {
				// miss
				sprintf(msg, "%s missed its attack...", curr_pokemon->get_species());
				io_display_message(msg, true);
			}

			// npc dead
			if (npc_pokemon->get_hp() == 0) {
				sprintf(msg, "You defeated the trainer's %s...", npc_pokemon->get_species());
				io_display_message(msg, true);

				// Check if any more valid pokemon
				if (c->firstAvailablePokemon() == nullptr) {
					world.pc->money += c->money;
					sprintf(msg, "%s has no more undefeated pokemon... you win! +%d pokebucks", c->getName(), c->money);
					io_display_message(msg, true);
					battle_over = 1;
				} else {
					npc_pokemon = c->firstAvailablePokemon();
					io_display_pokemon_summary(win_opponent_summary, npc_pokemon);
					sprintf(msg, "%s subbed in %s...", c->getName(), npc_pokemon->get_species());
					io_display_message(msg, true);
				}
			}
				// Wild turn
			else {
				if (rand() % 100 < moves[npc_pokemon->move_index[npc_move]].accuracy) {
					dmg = calculate_damage(npc_pokemon, curr_pokemon, npc_move);
					curr_pokemon->take_damage(dmg);
					io_display_pokemon_summary(win_player_summary, curr_pokemon);
					sprintf(msg, "The trainer's %s did %d damage to your %s...", npc_pokemon->get_species(), dmg, curr_pokemon->get_species());
					io_display_message(msg, true);
				} else {
					// miss
					sprintf(msg, "%s missed its attack...", npc_pokemon->get_species());
					io_display_message(msg, true);
				}

				// Curr dead
				if (curr_pokemon->get_hp() == 0) {
					sprintf(msg, "The trainer's %s defeated your %s...", npc_pokemon->get_species(), curr_pokemon->get_species());
					io_display_message(msg, true);

					// Check if any more valid pokemon
					if (pc->firstAvailablePokemon() == nullptr) {
						io_display_message("You have no more undefeated pokemon...", true);
						battle_over = 1;
					} else {
						curr_pokemon = pc->firstAvailablePokemon();
						io_display_pokemon_summary(win_player_summary, curr_pokemon);
						sprintf(msg, "%s has been subbed in...", curr_pokemon->get_species());
						io_display_message(msg, true);
					}
				}
			}
		}

		else { // Wild pokemon first
			if (rand() % 100 < moves[npc_pokemon->move_index[npc_move]].accuracy) {
				dmg = calculate_damage(npc_pokemon, curr_pokemon, npc_move);
				curr_pokemon->take_damage(dmg);
				io_display_pokemon_summary(win_player_summary, curr_pokemon);
				sprintf(msg, "The trainer's %s did %d damage to your %s...", npc_pokemon->get_species(), dmg,
						curr_pokemon->get_species());
				io_display_message(msg, true);
			} else {
				// miss
				sprintf(msg, "%s missed its attack...", npc_pokemon->get_species());
				io_display_message(msg, true);
			}
			// Curr dead
			if (curr_pokemon->get_hp() == 0) {
				sprintf(msg, "The trainer's %s defeated your %s...", npc_pokemon->get_species(), curr_pokemon->get_species());
				io_display_message(msg, true);

				// Check if any more valid pokemon
				if (pc->firstAvailablePokemon() == nullptr) {
					io_display_message("You have no more undefeated pokemon...", true);
					battle_over = 1;
				} else {
					curr_pokemon = pc->firstAvailablePokemon();
					io_display_pokemon_summary(win_player_summary, curr_pokemon);
					sprintf(msg, "%s has been subbed in...", curr_pokemon->get_species());
					io_display_message(msg, true);
				}
			}

				// Pc pokemon turn
			else {
				if (pc_move != nop) {
					if (rand() % 100 < moves[curr_pokemon->move_index[pc_move]].accuracy) {
						dmg = calculate_damage(curr_pokemon, npc_pokemon, pc_move);
						npc_pokemon->take_damage(dmg);
						io_display_pokemon_summary(win_opponent_summary, npc_pokemon);
						sprintf(msg, "%s did %d damage to the wild %s...", curr_pokemon->get_species(), dmg,
								npc_pokemon->get_species());
						io_display_message(msg, true);
					} else {
						// miss
						sprintf(msg, "%s missed its attack...", curr_pokemon->get_species());
						io_display_message(msg, true);
					}

					// Wild dead
					if (npc_pokemon->get_hp() == 0) {
						sprintf(msg, "You defeated the trainer's %s...", npc_pokemon->get_species());
						io_display_message(msg, true);

						// Check if any more valid pokemon
						if (c->firstAvailablePokemon() == nullptr) {
							world.pc->money += c->money;
							sprintf(msg, "%s has no more undefeated pokemon... you win! +%d pokebucks", c->getName(), c->money);
							io_display_message(msg, true);
							battle_over = 1;
						} else {
							npc_pokemon = c->firstAvailablePokemon();
							io_display_pokemon_summary(win_opponent_summary, npc_pokemon);
							sprintf(msg, "%s subbed in %s...", c->getName(), npc_pokemon->get_species());
							io_display_message(msg, true);
						}
					}
				}
			}
		}
	} // while battle not over
	hide_panel(panels[win_opponent_summary]);
	hide_panel(panels[win_player_summary]);
	hide_panel(panels[win_battle]);
	wclear(windows[win_battle]);
	wclear(windows[win_top]);
	c->defeated = 1;
}

static void io_heal_all() {
	int i;
	for (i = 0; i < world.pc->pokemon.size(); i++) {
		world.pc->pokemon.at(i)->set_hp(world.pc->pokemon.at(i)->get_max_hp());
	}
}

static void io_display_center() {
	show_panel(panels[win_center]);
	top_panel(panels[win_center]);
	wclear(windows[win_center]);


	// show party pokemon
	int i;
	int party_x = 1;
	int party_y = TERMINAL_Y-10;
	mvwprintw(windows[win_center], party_y, party_x, "Party Pokemon:");
	for (i = 0; i < world.pc->pokemon.size() && i < 6; i++) {
		mvwprintw(windows[win_center], party_y+1+i, party_x, "%c) %.10s Lv:%d HP:%d/%d",
				  '0' + i,
				  world.pc->pokemon.at(i)->get_species(),
				  world.pc->pokemon.at(i)->get_level(),
				  world.pc->pokemon.at(i)->get_hp(),
				  world.pc->pokemon.at(i)->get_max_hp());
	}

	int box_x = 40;
	int box_y = 1;
	mvwprintw(windows[win_center], box_y, box_x, "Box Pokemon:");
	for (i = 0; i < world.pc->box.size() && i < 24; i++) {
		mvwprintw(windows[win_center], box_y+1+i, box_x, "%c) %.10s Lv:%d HP:%d/%d",
				  'a' + i,
				  world.pc->box.at(i)->get_species(),
				  world.pc->box.at(i)->get_level(),
				  world.pc->box.at(i)->get_hp(),
				  world.pc->box.at(i)->get_max_hp());
	}
	update_panels();
	doupdate();
}



static void swap_box_pokemon(int pokemon_slot) {
	int done;
	int key;
	show_panel(panels[win_mart]);
	top_panel(panels[win_mart]);
	done = 0;
	while(!done) {
		wclear(windows[win_mart]);
		io_display_message(
				"Select pokemon in party to swap",
				false);
		// redisplay pokemart
		io_display_center();

		flushinp();
		key = getch();
		switch (key) {
			case '0':
				if (world.pc->pokemon.size() < 1) {
					io_display_message("You do not have a pokemon in that slot...", true);
				} else {
					// Swap pokemon_slot with curr_pokemon
					Pokemon* temp = world.pc->pokemon[0];
					world.pc->pokemon[0] = world.pc->box.at(pokemon_slot);
					world.pc->box.at(pokemon_slot) = temp;
				}
				done = 1;
				break;
			case '1':
				if (world.pc->pokemon.size() < 2) {
					io_display_message("You do not have a pokemon in that slot...", true);
				} else {
					// Swap pokemon_slot with curr_pokemon
					Pokemon* temp = world.pc->pokemon[1];
					world.pc->pokemon[1] = world.pc->box.at(pokemon_slot);
					world.pc->box.at(pokemon_slot) = temp;
				}
				done = 1;
				break;
			case '2':
				if (world.pc->pokemon.size() < 3) {
					io_display_message("You do not have a pokemon in that slot...", true);
				} else {
					// Swap pokemon_slot with curr_pokemon
					Pokemon* temp = world.pc->pokemon[2];
					world.pc->pokemon[2] = world.pc->box.at(pokemon_slot);
					world.pc->box.at(pokemon_slot) = temp;
				}
				done = 1;
				break;
			case '3':
				if (world.pc->pokemon.size() < 4) {
					io_display_message("You do not have a pokemon in that slot...", true);
				} else {
					// Swap pokemon_slot with curr_pokemon
					Pokemon* temp = world.pc->pokemon[3];
					world.pc->pokemon[3] = world.pc->box.at(pokemon_slot);
					world.pc->box.at(pokemon_slot) = temp;
				}
				done = 1;
				break;
			case '4':
				if (world.pc->pokemon.size() < 5) {
					io_display_message("You do not have a pokemon in that slot...", true);
				} else {
					// Swap pokemon_slot with curr_pokemon
					Pokemon* temp = world.pc->pokemon[4];
					world.pc->pokemon[4] = world.pc->box.at(pokemon_slot);
					world.pc->box.at(pokemon_slot) = temp;
				}
				done = 1;
				break;
			case '5':
				if (world.pc->pokemon.size() < 6) {
					io_display_message("You do not have a pokemon in that slot...", true);
				} else {
					// Swap pokemon_slot with curr_pokemon
					Pokemon* temp = world.pc->pokemon[5];
					world.pc->pokemon[5] = world.pc->box.at(pokemon_slot);
					world.pc->box.at(pokemon_slot) = temp;
				}
				done = 1;
				break;
			default:
				io_display_message("Not a valid action. Please swap pokemon first...", true);
				done = 0;
				break;
		}
	}
}



static void io_center() {
	int done;
	int key;
	int swapped = 0;
	show_panel(panels[win_center]);
	top_panel(panels[win_center]);
	done = 0;
	while(!done) {
		wclear(windows[win_center]);
		io_display_message("Your pokemon have been healed. Select a pokemon in box to swap to party", false);
		io_heal_all();
		io_display_center();

		flushinp();
		key = getch();
		int i;
		for (i = 0; i < world.pc->box.size(); i++) {
			if (key == 97+i) {
				swap_box_pokemon(i);
				done = 0;
				swapped = 1;
				break;
			}
		}
		if (swapped == 0 && key == 27) {
			done = 1;
			break;
		} else if (swapped == 0) {
			io_display_message("Invalid action. ESC to leave or letter to swap pokemon...", true);
		}
		swapped = 0;
	}
	wclear(windows[win_center]);
	wclear(windows[win_center]);
	wclear(windows[win_top]);
	hide_panel(panels[win_center]);
}

static void io_mart() {
	int done;
	int key;
	int swapped = 0;
	show_panel(panels[win_center]);
	top_panel(panels[win_center]);
	done = 0;
	while(!done) {
		wclear(windows[win_center]);
		io_display_message("Your pokemon have been healed. Select a pokemon in box to swap to party", false);
		io_heal_all();
		io_display_center();

		flushinp();
		key = getch();

		switch (key) {
			// NorthWest
			case 'x':
				break;
			case 'y':
				break;
			case 'z':
				break;
			default:
				break;
		}
		

	}
	wclear(windows[win_center]);
	wclear(windows[win_top]);
	hide_panel(panels[win_center]);
}

void io_player_turn() {
	int key;
	int turn_consumed;
	int moving;
	pos_t pcPos;
	pos_t newPos;

	static int random_encounter_buffer_count = RANDOM_ENCOUNTER_BUFFER;
	turn_consumed = 0;
	moving = 0;
	pcPos = world.pc->pos;
	newPos = world.pc->pos;

	while (!turn_consumed) {
		show_panel(panels[win_map]);
		top_panel(panels[win_map]);
		io_display_map();
		flushinp();
		key = getch();
		io_clear_message();
		switch (key) {
			// NorthWest
			case '7':   // 7
			case 'y':   // y
			case KEY_HOME:
				newPos = (pos_t){pcPos.x + all_dirs[dir_northwest].x, pcPos.y + all_dirs[dir_northwest].y};
				moving = 1;
				break;
				// North
			case '8':    // 8
			case 'k':    // k
			case KEY_UP:
				newPos = (pos_t){pcPos.x + all_dirs[dir_north].x, pcPos.y + all_dirs[dir_north].y};
				moving = 1;
				break;
				// NorthEast
			case '9':    // 9
			case 'u':    // u
			case KEY_PPAGE:
				newPos = (pos_t){pcPos.x + all_dirs[dir_northeast].x, pcPos.y + all_dirs[dir_northeast].y};
				moving = 1;
				break;
				// West
			case '4':    // 4
			case 'h':    // h
			case KEY_LEFT:
				newPos = (pos_t){pcPos.x + all_dirs[dir_west].x, pcPos.y + all_dirs[dir_west].y};
				moving = 1;
				break;
				// East
			case '6':    // 6
			case 'l':    // l
			case KEY_RIGHT:
				newPos = (pos_t){pcPos.x + all_dirs[dir_east].x, pcPos.y + all_dirs[dir_east].y};
				moving = 1;
				break;
				// SouthWest
			case '1':    // 1
			case 'b':    // b
			case KEY_END:
				newPos = (pos_t){pcPos.x + all_dirs[dir_southwest].x, pcPos.y + all_dirs[dir_southwest].y};
				moving = 1;
				break;
				// South
			case '2':    // 2
			case 'j':    // j
			case KEY_DOWN:
				newPos = (pos_t){pcPos.x + all_dirs[dir_south].x, pcPos.y + all_dirs[dir_south].y};
				moving = 1;
				break;
				// SouthEast
			case '3':    // 3
			case 'n':    // n
			case KEY_NPAGE:
				newPos = (pos_t){pcPos.x + all_dirs[dir_southeast].x, pcPos.y + all_dirs[dir_southeast].y};
				moving = 1;
				break;

				// NOP
			case '5':    // 5
			case ' ':    // space
			case '.':    // .
			case KEY_B2:
				turn_consumed = 1;
				moving = 0;
				break;

				// Enter a building
			case '>':    // >
				if (world.cur_map->terM[world.pc->pos.y][world.pc->pos.x] == ter_mart) {
					turn_consumed = 0;
					io_mart();
				}
				else if (world.cur_map->terM[world.pc->pos.y][world.pc->pos.x] == ter_center) {
					turn_consumed = 0;
					io_center();
				} else {
					turn_consumed = 0;
					io_display_message("You are not on top of a building...", true);
				}
				break;

				// List of trainers
			case 't':    // t
				io_list_trainers();
				moving = 0;
				turn_consumed = 0;
				break;

				// Quit the game
			case 'q':    // q
			case 'Q':	 // Q
				turn_consumed = 1;
				world.quit = 1;
				break;
			case 'B':
			case 'i':
				io_bag();
				turn_consumed = 0;
				break;

			case 27:    // esc
			default:
				wclear(windows[win_top]);
				mvwaddstr(windows[win_top], 0, 0, "Invalid action.");
		} // switch(key)
		if (world.quit) {
			return;
		}

		// PC can't move to that terrain
		if (newPos != pcPos && move_cost[char_pc][w_terpos(newPos)] == INT_MAX) {
			wclear(windows[win_top]);
			mvwaddstr(windows[win_top], 0, 0, "You can't move there.");
		}
		if (newPos != pcPos && move_cost[char_pc][w_terpos(newPos)] != INT_MAX) {
			if (w_charpos(newPos) && ((Npc*)w_charpos(newPos))->defeated) {
				wclear(windows[win_top]);
				mvwaddstr(windows[win_top], 0, 0, "That trainer has already been defeated.");
			}

			// NPC Battle
			if (w_charpos(newPos) && !((Npc*)w_charpos(newPos))->defeated) {
				io_trainer_battle((Npc*)w_charpos(newPos));
			}

			// Move tiles
			if (!w_charpos(newPos) && w_terpos(newPos) != ter_exit) {
				move_char((Character*)(world.pc), newPos);
				// Check for random encounter (10%)
				if (w_terpos(newPos) == ter_grass && random_encounter_buffer_count < 1 && rand() % 10 == 0) {
					io_random_encounter();
					random_encounter_buffer_count = RANDOM_ENCOUNTER_BUFFER;
				}
				random_encounter_buffer_count--;
				turn_consumed = 1;
			}

			// Move maps
			if (w_terpos(newPos) == ter_exit) {
				if (newPos.y < 1) { 		// N
					world_changeMap((Pos){world.cur_idx.x, world.cur_idx.y - 1}, world.cur_idx);
				}
				if (newPos.y >= MAP_Y - 1) { // S
					world_changeMap((Pos){world.cur_idx.x, world.cur_idx.y + 1}, world.cur_idx);
				}
				if (newPos.x >= MAP_X - 1) { // E
					world_changeMap((Pos){world.cur_idx.x + 1, world.cur_idx.y}, world.cur_idx);
				}
				if (newPos.x < 1) {        // W
					world_changeMap((Pos) {world.cur_idx.x - 1, world.cur_idx.y}, world.cur_idx);
				}
				if (((Character *)heap_peek_min(&world.cur_map->turn)) != NULL) {
					world.pc->next_turn = ((Character *) heap_peek_min(&world.cur_map->turn))->next_turn;
					turn_consumed = 1;
				}
			}
		}
	} // while (!done)

	// calculate next turn and reinsert into
	// If the PC moved to a new map, it's next turn is the minimum in the heap + the tile cost
	world.pc->next_turn += move_cost[char_pc][w_terpos(world.pc->pos)];
	heap_insert(&world.cur_map->turn, world.pc);
}

#include "character.h"
const int move_cost[num_character_types][num_terrain_types] = {
		{ INT_MAX, INT_MAX, INT_MAX, 10, 10, 20, 10, INT_MAX, INT_MAX, 10, 10, INT_MAX, INT_MAX, INT_MAX },
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 15, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 15, 15, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX},
		{ INT_MAX, INT_MAX, INT_MAX, INT_MAX,10, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX}
};

/*
 * Compare movement cost of tile currently on
 */
int32_t turn_cmp(const void *key, const void *with) {
	int key_moveCost = ((Character *) key)->next_turn;
	int with_moveCost = ((Character *) with)->next_turn;
	return key_moveCost - with_moveCost;
}

/*
 * Delete character from heap
 */
void heap_delete_char(void *v) {
	if (v == world.pc) {
		free(world.pc);
		//world.pc = NULL;
	} else {
		free(v);
		//v = NULL;
	}
}

/*
 * Gives NPC a random direction of the 8 directions
 */
inline void rand_dir(Npc *c) {
	int i = rand() & 0x7;
	c->dir = all_dirs[i];
}

void move_char(Character *c, pos_t pos) {
	// if moving the PC to itself, then check if the PC exists in the charmap somewhere else other than it's position
	// Then move that PC position to its current position
	if (c->type == char_pc && !charpos(c->pos) && (pos.x == c->pos.x && pos.y == c->pos.y)) {
		charpos(pos) = world.pc;
		world.pc->pos = pos;
	}

	if (c->type == char_pc && !charpos(c->pos) && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		charpos(pos) = world.pc;
		world.pc->pos = pos;
	}
	if (c->type == char_pc && charpos(c->pos) && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		charpos(pos) = world.pc;
		charpos(c->pos) = NULL;
		world.pc->pos = pos;
	}

	// Set character position in character map
	if (c->type != char_pc && (pos.x != c->pos.x || pos.y != c->pos.y)) {
		charpos(pos) = charpos(c->pos);
		charpos(c->pos) = NULL;
		// Set character inner position
		charpos(pos)->pos = pos;
	}
}

/*
 * Assumes the PC was removed from the map if previously visisted
 *
 */
void pc_init(pos_t relative_toDir) {

	// Set pc position to correct exit location based on the previous map
	if (relative_toDir.x == all_dirs[dir_north].x && relative_toDir.y == all_dirs[dir_north].y) {
		world.pc->pos.x = world.cur_map->south;
		world.pc->pos.y = MAP_Y - 2;
		move_char(world.pc, world.pc->pos);
	}
	else if (relative_toDir.x == all_dirs[dir_east].x && relative_toDir.y == all_dirs[dir_east].y) {
		world.pc->pos.x = 1;
		world.pc->pos.y = world.cur_map->west;
		move_char(world.pc, world.pc->pos);
	}
	else if (relative_toDir.x == all_dirs[dir_west].x && relative_toDir.y == all_dirs[dir_west].y) {
		world.pc->pos.x = MAP_X - 2;
		world.pc->pos.y = world.cur_map->east;
		move_char(world.pc, world.pc->pos);
	}
	else if (relative_toDir.x == all_dirs[dir_south].x && relative_toDir.y == all_dirs[dir_south].y) {
		world.pc->pos.x = world.cur_map->north;
		world.pc->pos.y = 1;
		move_char(world.pc, world.pc->pos);
	}
	else { // World Init PC
		pos_t pos;
		do {
			rand_pos(&pos);
			// Redundant check for char_m being null since this func will be called before NPCS
		} while (mappos(pos) != ter_path || world.cur_map->char_m[pos.y][pos.x] != NULL);

		if (!world.pc) {
			world.pc = (Pc*)malloc(sizeof(*(world.cur_map->char_m)));
			world.pc->pos = (pos_t) {pos.x, pos.y};
			charpos(world.pc->pos) = world.pc;
			world.pc->next_turn = 0;
			heap_insert(&world.cur_map->turn, world.pc);
		} else {
			world.pc->pos = (pos_t) {pos.x, pos.y};
			move_char(world.pc, world.pc->pos);
		}
	}

}

/*
 * Does not free the PC, just removes it from the charmap
 */
void pc_remove(map_t *map) {
	int x, y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			if (charxy(x,y) && charxy(x,y)->type == char_pc) {
				charxy(x,y) = NULL;
				return;
			}
		}
	}
}

character_type_t npc_get_random() {
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


/*
 * Places NPC characters in map according to global num_characters
 * Also places NPCs in map heap
 */
void npc_init(){
	pos_t pos;
	character_type_t new_char;
	int count;
	map_t *map = world.cur_map; // Redundant, but using it for char(x,y) to work
	// decrement count till zero, adding a random character each time
	for (count = num_trainers; count > 0; count--){

		// Always place a rival and hiker when possible
		// This if block is icky lol
		if 		(count == num_trainers)	   { new_char = char_rival; }
		else if (count == num_trainers - 1){ new_char = char_hiker; }
		else 	{new_char = npc_get_random(); }

		do {
			rand_pos(&pos);
		} while (charpos(pos) ||
				 move_cost[new_char][mappos(pos)] == INT_MAX ||
				 mappos(pos) == ter_path);

		charpos(pos) 			= (Character*)malloc(sizeof(*(map->char_m)));
		charpos(pos)->pos 		= (pos_t){pos.x,pos.y};
		charpos(pos)->type 		= new_char;
		charpos(pos)->next_turn = 0;
		((Npc*)charpos(pos))->defeated = 0;
		if(new_char == char_pc) {

		}
		else if (new_char == char_statue || new_char == char_hiker  || new_char == char_rival) {
			((Npc*)charpos(pos))->dir 	= (pos_t){0,0};
		} else {
			((Npc*)charpos(pos))->dir 	= all_dirs[rand() % dir_num];
		}
		heap_insert(&world.cur_map->turn, charpos(pos));
	}
}

void npc_battle(Npc *c) {
	// Battle
	int done;
	int key;
	done  = 0;
	// Right now, display to panel
	show_panel(panels[win_battle]);
	wclear(windows[win_battle]);
	while (!done) {
		box(windows[win_battle],0,0);
		mvwaddstr(windows[win_battle], 4,4,"BATTLE: Press ESC to defeat your opponent!");

		update_panels();
		doupdate();
		key = getch();
		switch (key) {
			case 27: // ESC
				c->defeated = 1;
				wclear(windows[win_battle]);
				hide_panel(panels[win_battle]);
				// PC wins
				// Mark trainer as defeated
				// Return to map
				done = 1;
				break;
		}
	}
}

void char_rivalTurn(Npc *c) {
	pos_t dest;
	int min;
	int base;
	int i;

	base = rand() & 0x7;
	dest = c->pos;
	min = INT_MAX;
	for (i = base; i < dir_num + base; i++) {
		if ((world.rival_dist[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] < min)){

			dest.x = c->pos.x + all_dirs[i & 0x7].x;
			dest.y = c->pos.y + all_dirs[i & 0x7].y;
			min = world.rival_dist[dest.y][dest.x];
		}
	}

	if (!c->defeated && charpos(dest) && charpos(dest)->type == char_pc) {
		npc_battle(c);
	}
	if (!c->defeated && !world.cur_map->char_m[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);

}
void char_hikerTurn(Npc *c) {
	pos_t dest;
	int min;
	int base;
	int i;

	base = rand() & 0x7;
	dest = c->pos;
	min = INT_MAX;
	for (i = base; i < dir_num + base; i++) {
		if ((world.rival_dist[c->pos.y + all_dirs[i & 0x7].y][c->pos.x + all_dirs[i & 0x7].x] < min)){

			dest.x = c->pos.x + all_dirs[i & 0x7].x;
			dest.y = c->pos.y + all_dirs[i & 0x7].y;
			min = world.hiker_dist[dest.y][dest.x];
		}
	}

	if (!c->defeated && charpos(dest) && charpos(dest)->type == char_pc) {
		npc_battle(c);
	}
	if (!c->defeated && !world.cur_map->char_m[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);

}
void char_statueTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;
	if (!world.cur_map->char_m[dest.y][dest.x]) {
		move_char(c, dest);
	}
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
void char_pacerTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x] !=
		 world.cur_map->m[c->pos.y][c->pos.x]) ||
		world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		c->dir.x *= -1;
		c->dir.y *= -1;
	}

	if ((world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x] ==
		 world.cur_map->m[c->pos.y][c->pos.x]) &&
		!world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}
	move_char(c, dest);
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
void char_wandererTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x] !=
		 world.cur_map->m[c->pos.y][c->pos.x]) ||
		world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		rand_dir(c);
	}

	if ((world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x] ==
		 world.cur_map->m[c->pos.y][c->pos.x]) &&
		!world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}
	move_char(c, dest);
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}
void char_randomTurn(Npc *c) {
	pos_t dest;
	dest = c->pos;

	if ((move_cost[c->type][world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]] == INT_MAX) ||
		world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		c->dir.x *= -1;
		c->dir.y *= -1;
	}

	if ((move_cost[c->type][world.cur_map->m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]] != INT_MAX) &&
		!world.cur_map->char_m[c->pos.y + c->dir.y][c->pos.x + c->dir.x]) {
		dest.x = c->pos.x + c->dir.x;
		dest.y = c->pos.y + c->dir.y;
	}

	move_char(c, dest);
	c->next_turn += move_cost[c->type][mappos(c->pos)];
	heap_insert(&world.cur_map->turn, c);
}


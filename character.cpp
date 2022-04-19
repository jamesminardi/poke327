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

const char *char_type_name[num_character_types] = {
		"PC",
		"Rival",
		"Hiker",
		"Trainer",
		"Pacer",
		"Wanderer",
		"Trainer"
};

Character::Character(character_type_t type, int x, int y, int next_turn) : type(type), pos(x,y), next_turn(next_turn) {}

Pc::Pc() : Character(char_pc, 0, 0, 0) {}

void Pc::move(Map* map, Pos pos) {
	// Swaps PC with char at pos
	Character* tmp = map->getChar(pos);
	map->setChar(pos, this);
	map->setChar((*this).pos, tmp);
	if (tmp) {
		tmp->pos = (*this).pos;
	}
	(*this).pos = pos;
}
void Pc::move(Map* map, int x, int y) {
	this->move(map, {x,y});
}

Npc::Npc(character_type_t type, int x, int y, int next_turn) : Character(type, x, y, next_turn) {
	this->defeated = 0;
	if (this->type == char_statue || this->type == char_hiker  || this->type == char_rival) {
		this->dir = {0, 0};
	} else {
		this->dir = dirs[rand() % dir_num];
	}
}
void Npc::move(Map* map, int x, int y) {
	this->move(map, {x,y});
}
void Npc::move(Map* map, Pos pos) {
	// cant move onto tile with pc
	if (map->getChar(pos) && map->getChar(pos)->type == char_pc) {
		return;
	}
	// Swaps PC with char at pos
	Character* tmp = map->getChar(pos);
	map->setChar(pos, this);
	map->setChar((*this).pos, tmp);
	if (tmp) {
		tmp->pos = (*this).pos;
	}
	(*this).pos = pos;
}
/*
 * Gives NPC a random direction of the 8 directions
 */
void Npc::rand_dir() {
	int i = rand() & 0x7;
	this->dir = dirs[i];
}


void Npc::rivalTurn(Map* map) {
	Pos dest;
	int min;
	int base;
	int i;

	base = rand() & 0x7;
	dest = this->pos;
	min = INT_MAX;
	for (i = base; i < dir_num + base; i++) {
		if ((world.rival_dist[this->pos.y + dirs[i & 0x7].y][this->pos.x + dirs[i & 0x7].x] < min)){
			dest.x = this->pos.x + all_dirs[i & 0x7].x;
			dest.y = this->pos.y + all_dirs[i & 0x7].y;
			min = world.rival_dist[dest.y][dest.x];
		}
	}

	if (!this->defeated && map->getChar(dest) && map->getChar(dest)->type == char_pc) {
		npc_battle(this);
	}
	if (!this->defeated && !map->getChar(dest)) {
		this->move(world.cur_map, dest);
	}
	this->next_turn += move_cost[this->type][map->getTerrain(this->pos)];
	heap_insert(&map->turn, this);

}

void Npc::hikerTurn(Map* map) {
	Pos dest;
	int min;
	int base;
	int i;

	base = rand() & 0x7;
	dest = this->pos;
	min = INT_MAX;
	for (i = base; i < dir_num + base; i++) {
		if ((world.rival_dist[this->pos.y + dirs[i & 0x7].y][this->pos.x + dirs[i & 0x7].x] < min)){
			dest.x = this->pos.x + all_dirs[i & 0x7].x;
			dest.y = this->pos.y + all_dirs[i & 0x7].y;
			min = world.rival_dist[dest.y][dest.x];
		}
	}

	if (!this->defeated && map->getChar(dest) && map->getChar(dest)->type == char_pc) {
		npc_battle(this);
	}
	if (!this->defeated && !map->getChar(dest)) {
		this->move(world.cur_map, dest);
	}
	this->next_turn += move_cost[this->type][map->getTerrain(this->pos)];
	heap_insert(&map->turn, this);
}
void Npc::statueTurn(Map* map) {
	Pos dest;
	dest = this->pos;
	if (!map->getChar(dest)) {
		this->move(map, dest);
	}
	this->next_turn += move_cost[this->type][map->getTerrain(this->pos)];
	heap_insert(&map->turn, this);
}
void Npc::pacerTurn(Map* map) {
	Pos dest;
	dest = this->pos;

	if ((map->getTerrain(this->pos + this->dir) != map->getTerrain(this->pos)) || map->getChar(this->pos + this->dir)) {
		this->dir.x *= -1;
		this->dir.y *= -1;
	}

	if ((map->getTerrain(this->pos + this->dir) == map->getTerrain(this->pos + this->dir)) && !map->getChar(this->pos + this->dir)) {
		dest.x = this->pos.x + this->dir.x;
		dest.y = this->pos.y + this->dir.y;
	}
	this->move(map, dest);
	this->next_turn += move_cost[this->type][map->getTerrain(this->pos)];
	heap_insert(&map->turn, this);
}
void Npc::wandererTurn(Map* map) {
	Pos dest;
	dest = this->pos;

	if ((map->getTerrain(this->pos + this->dir) != map->getTerrain(this->pos)) || map->getChar(this->pos + this->dir)) {
		this->rand_dir();
	}

	if ((map->getTerrain(this->pos + this->dir) == map->getTerrain(this->pos)) && !map->getChar(this->pos + this->dir)) {
		dest.x = this->pos.x + this->dir.x;
		dest.y = this->pos.y + this->dir.y;
	}
	this->move(map, dest);
	this->next_turn += move_cost[this->type][map->getTerrain(this->pos)];
	heap_insert(&map->turn, this);
}

void Npc::randomTurn(Map* map) {
	Pos dest;
	dest = this->pos;

	if ((move_cost[this->type][map->getTerrain(this->pos + this->dir)] == INT_MAX) ||
			map->getChar(this->pos + this->dir)) {
		this->dir.x *= -1;
		this->dir.y *= -1;
	}

	if ((move_cost[this->type][map->getTerrain(this->pos + this->dir)] != INT_MAX) &&
			!map->getChar(this->pos + this->dir)) {
		dest.x = this->pos.x + this->dir.x;
		dest.y = this->pos.y + this->dir.y;
	}

	this->move(map, dest);
	this->next_turn += move_cost[this->type][map->getTerrain(this->pos)];
	heap_insert(&map->turn, this);
}

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
	if (((Character*)v)->type == char_pc) {
		free(v);
		//world.pc = NULL;
	} else {
		free(v);
		//v = NULL;
	}
}

void pathfind(map_t *map, int char_dist[MAP_Y][MAP_X], const character_type_t character, const Pos start) {
	heap_t heap;
	static path_t *c;
	static path_t distance[MAP_Y][MAP_X];
	static int initialized = 0;
	int x, y;
	int i;

	pathfind_init_heap(&heap, character);

	if (!initialized) {
		initialized = 1;
		for (y = 0; y < MAP_Y; y++) {
			for (x = 0; x < MAP_X; x++) {
				distance[y][x].pos.y = y;
				distance[y][x].pos.x = x;
			}
		}
	}

	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			char_dist[y][x] = INT_MAX;
		}
	}

	// Set PC cost to 0
	//distance[start.y][start.x].cost = 0;
	char_dist[start.y][start.x] = 0;

	// Insert all infinity cost tiles into priority queue (entire map minus boundaries)
	for (y = 1; y < MAP_Y - 1; y++) {
		for (x = 1; x < MAP_X - 1; x++) {

			if (ter_cost(x,y,character) != INT_MAX) {
				distance[y][x].hn = heap_insert(&heap, &distance[y][x]);
			} else {
				distance[y][x].hn = NULL;
			}
		}
	}

	while ((c = (path_t*)heap_remove_min(&heap))) {
		c->hn = NULL; // mark as visited

		// For every neighbor of p, if not yet visited and cost is greater than current tile(p), then change it
		for (i = 0; i < dir_num; i++) {
			Pos nb = c->pos.getNeighbor((cardinal_dirs_t)i);

			if ((distance[nb.y][nb.x].hn) &&
				(char_dist[nb.y][nb.x] >
				(char_dist[c->pos.y][c->pos.x] + move_cost[character][map->m[c->pos.y][c->pos.x]])))
			{
				char_dist[nb.y][nb.x] = char_dist[c->pos.y][c->pos.x] + ter_cost(c->pos.x, c->pos.y, character);
				heap_decrease_key_no_replace(&heap, distance[nb.y][nb.x].hn);
			}
		}
	}
	heap_delete(&heap);
}
const char Character::toSymbol(){
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

const char *Character::toString(){
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

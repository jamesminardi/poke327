#include "map.h"

/*
 * Places all nodes on the map
 */
Map::Map() {
//	// Terrain and buildings
//	int x,y;
//	for (y = 0; y < MAP_Y; y++) {
//		for (x = 0; x < MAP_X; x++) {
//			this->charM[y][x] = NULL;
//			this->terM[y][x] = ter_clearing;
//		}
//	}
//	this->placeTree();
//	this->placeGrass();
//	this->placePath();
//	this->placeBorder();
//	this->placeExits();
//	this->placeCenter();
//	this->placeMart();
//
//	this->placeNpc();

}

/*
 * Initializes Map and places PC on map
 */
Map::Map(Pos pc_pos) {

}

Map::Map(int north, int south, int east, int west) : north(north), south(south), east(east), west(west) {
	// Terrain and buildings
	int x,y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			this->charM[y][x] = NULL;
			this->terM[y][x] = ter_clearing;
		}
	}
	this->placeTree();
	this->placeGrass();
	this->placePath();
	this->placeBorder();
	this->placeExits();
	this->placeCenter();
	this->placeMart();

	//this->placeNpc();

	heap_init(&this->turn, turn_cmp, heap_delete_char);
}

Map::~Map() {
	int x,y;
	for (y = 0; y < MAP_Y; y++) {
		for (x = 0; x < MAP_X; x++) {
			delete this->charM[y][x];
		}
	}
}

void Map::placeCharacters(Pc* pc){
	this->setChar(pc->pos, pc);
	heap_insert(&this->turn, pc);
	this->placeNpc();
}

Character* Map::getChar(int x, int y) {
	return this->charM[y][x];
}
Character* Map::getChar(Pos pos) {
	return this->charM[pos.y][pos.x];
}
void Map::setChar(int x, int y, Character* c) {
	this->charM[y][x] = c;
}
void Map::setChar(Pos pos, Character* c) {
	this->charM[pos.y][pos.x] = c;
}

terrain_t Map::getTerrain(int x, int y) {
	return this->terM[y][x];
}
terrain_t Map::getTerrain(Pos pos) {
	return this->terM[pos.y][pos.x];
}
void Map::setTerrain(int x, int y, terrain_t ter) {
	this->terM[y][x] = ter;
}
void Map::setTerrain(Pos pos, terrain_t ter) {
	this->terM[pos.y][pos.x] = ter;
}

/*
 * Finds valid 2x2 building location coordinate.
 * Coordinate is the top left-most unit.
 */
void Map::find_validBuildingLocation(int *x, int *y) {
	do {
		*x = rand() % (MAP_X - 3) + 1;
		*y = rand() % (MAP_Y - 3) + 1;
		if ((((this->getTerrain(*x, 	  	*y - 1) == ter_path) && (this->getTerrain(*x + 1, *y - 1) == ter_path)) ||
			 ((this->getTerrain(*x - 1,	*y	 ) == ter_path) && (this->getTerrain(*x - 1, *y + 1) == ter_path)) ||
			 ((this->getTerrain(*x + 2,	*y	 ) == ter_path) && (this->getTerrain(*x + 2, *y + 1) == ter_path)) ||
			 ((this->getTerrain(*x,    	*y + 2) == ter_path) && (this->getTerrain(*x + 2, *y + 2) == ter_path)))&&

			(((this->getTerrain(*x, 	 	*y	 ) != ter_mart) && (this->getTerrain(*x, 	*y) 	   != ter_center)  &&
			  (this->getTerrain(*x + 1, 	*y	 ) != ter_mart) && (this->getTerrain(*x + 1, *y) 	   != ter_center)  &&
			  (this->getTerrain(*x, 	 	*y + 1) != ter_mart) && (this->getTerrain(*x, 	*y + 1) != ter_center)  &&
			  (this->getTerrain(*x + 1, 	*y + 1) != ter_mart) && (this->getTerrain(*x + 1, *y + 1) != ter_center)))&&

			(((this->getTerrain(*x, 	 	*y)	   != ter_path) &&
			  (this->getTerrain(*x + 1, 	*y)	   != ter_path) &&
			  (this->getTerrain(*x, 	 	*y + 1) != ter_path) &&
			  (this->getTerrain(*x + 1, 	*y + 1) != ter_path)))) {
			break;
		}
	} while (1);
}

void Map::placeMart() {
	int x;
	int y;
	find_validBuildingLocation(&x, &y);
	this->setTerrain(	x, 		  y,      ter_mart);
	this->setTerrain(x + 1, 	  y, 	  ter_mart);
	this->setTerrain(x + 1, 	y + 1, ter_mart);
	this->setTerrain(	x,		y + 1, ter_mart);
}

void Map::placeCenter() {
	int x;
	int y;
	find_validBuildingLocation(&x, &y);
	this->setTerrain(	x, 		  y,      ter_center);
	this->setTerrain(x + 1, 	  y, 	  ter_center);
	this->setTerrain(x + 1, 	y + 1, ter_center);
	this->setTerrain(	x,		y + 1, ter_center);
}

/*
 * Places border walls with path exits
 */
void Map::placeBorder() {
	int y;
	int x;
	// left & right
	for (y = 0; y < MAP_Y; y++) {
		this->setTerrain(0, y, ter_border);
		this->setTerrain(MAP_X - 1, y, ter_border);
	}
	// top & bottom
	for (x = 0; x < MAP_X; x++) {
		this->setTerrain(x, 0, 		  ter_border);
		this->setTerrain(x, MAP_Y - 1, ter_border);
	}
}

void Map::placeExits() {
	setTerrain(north, 0, ter_exit);
	setTerrain(south, MAP_Y - 1, ter_exit);
	setTerrain(0, west, ter_exit);
	setTerrain(MAP_X - 1, east, ter_exit);
}

/*
 * Does not place path on border
 */
void Map::placePath() {
	int x_break, y_break;

	/* Find road paths that wont create a double road */
	do {
		x_break = rand() % (MAP_X - 6) + 3;
		y_break = rand() % (MAP_Y - 6) + 3;

	} while (abs(this->north - x_break) == 1 || abs(this->south - x_break == 1) ||
			 abs(this->west  - y_break) == 1 || abs(this->east  - y_break == 1));

	/* Place roads by connecting opposite exits together with the break */
	/* East to West */
	int i;
	for (i = 1; 	  i < x_break; 	 i++) { this->setTerrain(i, this->west, ter_path); }
	for (i = x_break; i < MAP_X - 1; i++) { this->setTerrain(i, this->east, ter_path); }

	if (this->west <  this->east) {
		for (i = this->west; i <  this->east; i++) { this->setTerrain(x_break, i, ter_path); }
	}
	if (this->west >= this->east) {
		for (i = this->east; i <= this->west; i++) { this->setTerrain(x_break, i, ter_path); }
	}
	/* North to South */
	for (i = 1; 	  i < y_break; 	 i++) { this->setTerrain(this->north, i, ter_path); }
	for (i = y_break; i < MAP_Y - 1; i++) { this->setTerrain(this->south, i, ter_path); }
	if (this->north <  this->south) {
		for (i = this->north; i <  this->south; i++) { this->setTerrain(i, y_break, ter_path); }
	}
	if (this->north >= this->south) {
		for (i = this->south; i <= this->north; i++) { this->setTerrain(i, y_break, ter_path); }
	}
}

/*
 * Places grass nodes in random locations on the map
 */
void Map::placeGrass() {
	int x_pos, y_pos, x_size, y_size;
	int node_count;
	int x, y;

	for (node_count = 0; node_count < GRASS_NODES; node_count++) {
		/* Places new grass node where there isn't already grass */
		do {
			x_pos = rand() % MAP_X;
			y_pos = rand() % MAP_Y;
		} while (this->getTerrain(x_pos, y_pos) == ter_grass);

		x_size = rand() % 5 + 4;
		y_size = rand() % 4 + 2;

		/* Stamps grass of random size onto node */
		for (y = y_pos - y_size; y < y_pos + y_size; y++) {
			for (x = x_pos - x_size; x < x_pos + x_size; x++) {
				if (y >= 0 && y < MAP_Y) {
					if (x >= 0 && x < MAP_X) {
						this->setTerrain(x, y, ter_grass);
					}
				}
			}
		}
	}
}

/*
 * Returns 0 if invalid position for a tree.
 * Single trees can not be adjacent to one another or forests
 */
int Map::is_validTree(int x, int y) {

	int valid = 1;

	if (x <= 0 || x >= MAP_X || y < 0 || y >= MAP_Y) {
		valid = 0;
	}

	if ((this->getTerrain(x, y) 	 	  	  == ter_tree) || (this->getTerrain(x, y + 1) 		== ter_tree) ||
		(this->getTerrain(x, y - 1) 		  == ter_tree) || (this->getTerrain(x + 1, y) 		== ter_tree) ||
		(this->getTerrain(x - 1, y) 	 	  == ter_tree) || (this->getTerrain(x - 1, y - 1) == ter_tree) ||
		(this->getTerrain(x + 1, y - 1) == ter_tree) || (this->getTerrain(x - 1, y + 1) == ter_tree) ||
		(this->getTerrain(x + 1, y + 1) == ter_tree)) {
		valid = 0;
	}
	if ((this->getTerrain(x, y) 	 	 	  == ter_forest) || (this->getTerrain(x, y + 1) 		  == ter_forest) ||
		(this->getTerrain(x, y - 1) 	 	  == ter_forest) || (this->getTerrain(x + 1, y) 		  == ter_forest) ||
		(this->getTerrain(x - 1, y) 	 	  == ter_forest) || (this->getTerrain(x - 1, y - 1) == ter_forest) ||
		(this->getTerrain(x + 1, y - 1) == ter_forest) || (this->getTerrain(x - 1, y + 1) == ter_forest) ||
		(this->getTerrain(x + 1, y + 1) == ter_forest)) {
		valid = 0;
	}
	return valid;
}

void Map::placeTree() {

	int x, y, n;
	for (n = 0; n < MAX_TREES; n++) {

		do {
			x = rand() % (MAP_X - 2) + 1;
			y = rand() % (MAP_Y - 2) + 1;
		} while (!is_validTree(x, y));

		this->setTerrain(x, y, ter_tree);
	}
}

void Map::placePc(Pc *pc, Pos relativeToDir){
	// Set pc position to correct exit location based on the previous map
	if (relativeToDir.x == dirs[dir_north].x && relativeToDir.y == dirs[dir_north].y) {
		pc->pos = {this->south, MAP_Y - 2};
		pc->move((map*)this, pc->pos);
	}
	else if (relativeToDir.x == dirs[dir_east].x && relativeToDir.y == dirs[dir_east].y) {
		pc->pos.x = 1;
		pc->pos.y = this->west;
		pc->move((map*)this, pc->pos);
	}
	else if (relativeToDir.x == dirs[dir_west].x && relativeToDir.y == dirs[dir_west].y) {
		pc->pos.x = MAP_X - 2;
		pc->pos.y = this->east;
		pc->move((map*)this, pc->pos);
	}
	else if (relativeToDir.x == dirs[dir_south].x && relativeToDir.y == dirs[dir_south].y) {
		pc->pos.x = this->north;
		pc->pos.y = 1;
		pc->move((map*)this, pc->pos);
	}
	else { // Fly / Teleport
		Pos pos;
		do {
			rand_pos(&pos);
			// Redundant check for char_m being null since this func will be called before NPCS
		} while (this->getTerrain(pos) != ter_path || this->getChar(pos) != nullptr);
		pc->pos = (Pos){pos.x, pos.y};
		pc->move((map*)this, pc->pos);

	}
}

/*
 * Places NPC characters in map according to global num_characters
 * Also places NPCs in map heap
 * Does not place NPC on terrain it can not path on or paths
 */
void Map::placeNpc(){
	Pos pos;
	character_type_t newChar;

	// decrement count till zero, adding a random character each time
	int count;
	for (count = num_trainers; count > 0; count--){

		// Always place a rival and hiker when possible
		// This block is icky lol
		if 		(count == num_trainers)	   { newChar = char_rival; }
		else if (count == num_trainers - 1){ newChar = char_hiker; }
		else 	{ newChar = npc_getRandom(); }

		do {
			rand_pos(&pos);
		} while (this->getChar(pos) ||
				 move_cost[newChar][this->getTerrain(pos)] == INT_MAX ||
				this->getTerrain(pos) == ter_path);
		this->setChar(pos, new Character(newChar, pos.x, pos.y, 0));
		heap_insert(&this->turn, this->getChar(pos));
	}
}

#ifndef POKE327_POS_H
#define POKE327_POS_H

#include <random>
#include "globals.h"


typedef enum cardinal_dirs {
	dir_northwest,
	dir_north,
	dir_northeast,
	dir_west,
	dir_east,
	dir_southwest,
	dir_south,
	dir_southeast,
	dir_num
} cardinal_dirs_t;

class Pos {
public:
	int x;
	int y;
	Pos();
	Pos(int x, int y);
	int manhattan(Pos p1, Pos p2);
	Pos getNeighbor(cardinal_dirs_t dir);
	Pos& operator+=(const Pos& other);
private:
};

/// Returns copy of result of (lhs + rhs)
///	lhs is updated after (lhs += rhs) and returned as a copy
inline Pos operator+(Pos lhs, const Pos& rhs)
{
	lhs += rhs;
	return lhs;
}

typedef struct pos {
	int x;
	int y;
} pos_t;

static Pos dirs[8] = {
		{ -1, -1 },
		{ 0,  -1 },
		{ 1,  -1 },
		{ -1, 0  },
		{ 1,  0  },
		{ -1, 1  },
		{ 0,  1  },
		{ 1,  1  },
};

static pos_t all_dirs[8] = {
		{ -1, -1 },
		{ 0,  -1 },
		{ 1,  -1 },
		{ -1, 0  },
		{ 1,  0  },
		{ -1, 1  },
		{ 0,  1  },
		{ 1,  1  },
};

/*
 * Set values in pos to a random coordinate within the playable area
 */
inline void rand_pos(Pos *pos) {
	pos->x = (rand() % (MAP_X - 2)) + 1;
	pos->y = (rand() % (MAP_Y - 2)) + 1;
}

#endif //POKE327_POS_H

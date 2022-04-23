#ifndef POKE327_POS_H
#define POKE327_POS_H

#include <cmath>

typedef struct pos {
	int x;
	int y;
	bool operator==(const pos& a) const
	{
		return (x == a.x && y == a.y);
	}
	bool operator!=(const pos& a) const
	{
		return (x != a.x || y != a.y);
	}
} pos_t;

inline int manhattan(pos_t p1, pos_t p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

#endif //POKE327_POS_H

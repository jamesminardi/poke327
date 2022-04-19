#include "pos.h"
#include <cmath>

Pos::Pos() : x(0), y(0) {}

Pos::Pos(int x, int y) : x(x), y(y) {}

int Pos::manhattan(const Pos p1, const Pos p2){
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

Pos& Pos::operator+=(const Pos& other)
{
	(*this).x = (*this).x + other.x;
	(*this).y = (*this).y + other.y;
	return *this;
}

Pos Pos::getNeighbor(cardinal_dirs_t dir) {
	return (*this) + dirs[dir];
}

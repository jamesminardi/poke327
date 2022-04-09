#include "pos.h"
#include <cmath>

Pos::Pos(int x, int y) : x(x), y(y) {}

int Pos::manhattan(const Pos p1, const Pos p2){
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

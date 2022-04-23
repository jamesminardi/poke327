#ifndef POKE327_POS_H
#define POKE327_POS_H

#include <cmath>

typedef struct Pos {
	int x;
	int y;
	bool operator==(const Pos& a) const
	{
		return (x == a.x && y == a.y);
	}
	bool operator!=(const Pos& a) const
	{
		return (x != a.x || y != a.y);
	}
	inline int manhattan(Pos p) {
		return abs(x - p.x) + abs(y - p.y);
	}
	/*
	 * returns relative position from caller object to parameter p
	 */
	inline Pos relative(Pos p) {
		Pos relativePos;
		relativePos.x = x - p.x;
		relativePos.y = y - p.y;
		return relativePos;
	}
} pos_t;

inline int manhattan(pos_t p1, pos_t p2) {
	return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

//static std::string relativePosString(int x, int y) {
//	pos_t relPos;
//	relPos.x = x - world.pc->pos.x;
//	relPos.y = y - world.pc->pos.y;
//	std::string str = "(" + std::to_string(relPos.x) + ", " + std::to_string(relPos.y) + ")";
//	return str;
//
//}

#endif //POKE327_POS_H

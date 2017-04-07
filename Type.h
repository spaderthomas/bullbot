#pragma once
#include <string>
#include <unordered_map>
struct Move;
struct Pokemon;
struct Type {
	enum : int {
		NORMAL = 0,
		GRASS = 1,
		FIRE = 2,
		WATER = 3,
		ELECTRIC = 4,
		FLYING = 5,
		GROUND = 6,
		ROCK = 7,
		ICE = 8,
		BUG = 9,
		POISON = 10,
		FIGHTING = 11,
		PSYCHIC = 12,
		GHOST = 13,
		DARK = 14,
		STEEL = 15,
		DRAGON = 16,
		FAIRY = 17,
		LIGHT = 18,
		NEUTRAL = 19
	};
	int flag = NORMAL;
	//todo: finish if necessary

	std::unordered_map<int, std::string> _typename = {
		{NORMAL, "Normal"}
	};
	std::string as_typename() {
		return std::string();
	}
	Type(const int &value) : flag(value){}
	operator int() {
		return flag;
	}
	static const float effectiveness[20][20];
	static float get_effectiveness(Move &a, Pokemon &b) {
		return 1;
	}
};

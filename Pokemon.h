#pragma once
#include <string>
#include <vector>
#include "Move.h"
#include "Type.h"
#include "Ability.h"
#include "Item.h"
struct Move;
struct Type;
struct Pokemon {
	std::string name;
	Ability ability;
	Item item;
	bool fainted = false;
	bool active = false;
	int number = 0;
	int level = 0;
	int attack = 0;
	int defense = 0;
	int speed = 0;
	int special = 0;
	Type type = Type::NORMAL;
	std::vector<Move> moveset;

	std::vector<float> as_attributes() {
		return {
			float(number),
			float(level),
			float(type),
			float(attack),
			float(defense),
			float(speed),
			float(special),
		};
	};
};
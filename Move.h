#pragma once
#include <string>
#include "Type.h"
#include "Battle.h"
#include "Pokemon.h"
struct Battle;
struct Move {
	bool isvalid = false;
	std::string name = "None";
	int number = 0;
	int power = 0;
	int pp = 1;
	Type type = Type::NORMAL;
	enum Category : char {
		STATUS,
		PHYSICAL,
		SPECIAL
	} category = Category::STATUS;
	/*
	void (*move_function) (Pokemon* user, Battle* battle, Pokemon* targets[], unsigned long long size) = nullptr;
	void onChosen(Pokemon* user, Battle* battle, Pokemon* targets[], unsigned long long targets_size) {
		if (move_function != nullptr) {
			move_function(user, battle, targets, targets_size);
		}
	}*/

	std::vector<float> as_attributes() {
		std::vector<float> attributes = {
			(float)number,
			(float)power,
			(float)type,
			(float)category
		};
		return attributes;
	}

};
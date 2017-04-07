#pragma once
#include <random>
#include "Pokemon.h"
#include "Move.h"
#include "Type.h"

std::default_random_engine generator;
std::uniform_real_distribution<float> r_factor(.85, 1);
float random_factor() {
	return (float)r_factor(generator);
}

float STAB(Pokemon &attacker, Move &attack) {
	return (attack.type & attacker.type) ? 1.5f : 1.f;
}


int attackDamage(Pokemon &attacker, Move &attack, Pokemon &defender) {
	return (int)((((attacker.level * 0.4f + 2.f) * attack.power * attacker.attack / defender.defense) / 50.f + 2.f)
		* STAB(attacker, attack) * Type::get_effectiveness(attack, defender) * random_factor());
}



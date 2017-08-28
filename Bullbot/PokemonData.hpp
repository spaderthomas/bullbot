#pragma once
#include <vector>
#include <string>
#include "MoveData.hpp"

struct PokemonData {
	bool active;
	std::string base_ability;
	short curr_condition;
	short best_condition;
	short lvl;
	std::string ident;
	std::string item;
	std::string pokeball;
	short attack;
	short defense;
	short special_attack;
	short special_defense;
	short speed;
	bool is_trapped = false;
	bool is_fainted = false;
	std::vector<MoveData> move_data;

	template<typename T>
	std::vector<T> to_float_vec() {
		int m_len = move_data.size();
		// is_active, if move 1, move 1 data, if move x, move x data, 
		return { (T)active, (mlen > 0 ? 1.f : 0.f),  (mlen > 1 ? 1.f : 0.f), (mlen > 2 ? 1.f : 0.f), (mlen > 3 ? 1.f : 0.f) };
	}
};
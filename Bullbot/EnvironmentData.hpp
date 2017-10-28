#pragma once
#include <vector>
#include <string>
#include "EnvironmentSettings.hpp"

template<typename T>
struct HasDataRepresentation {
	virtual std::vector<T> as_vector() = 0;
};

struct HasFloatDataRepresentation : HasDataRepresentation<float> {};

// float vector type
typedef std::vector<float> fvec_t;

struct BasicBattleData {
	bool game_over = false;
	fvec_t as_vector() {
		return {};
	}
};

struct MoveData {
	std::string name;
	short pp = 16;
	fvec_t as_vector() {
		return {
			(float)(move_index.count(name) ? move_index.at(name) : -1),
			(float)pp
		};
	}
};


struct PokemonData {
	std::string name = "";
	std::string ident = "";
	std::string item = "";
	std::string pokeball = "";
	std::string base_ability = "";
	bool active = false;
	short curr_condition = 0;
	short best_condition = 0;
	short lvl = 0;
	short attack = 0;
	short defense = 0;
	short special_attack = 0;
	short special_defense = 0;
	short speed = 0;
	bool is_trapped = false;
	bool is_fainted = false;
	std::vector<MoveData> move_data;
	enum KnowledgeState : short {
		UNKNOWN,
		KNOWN
	};

	fvec_t as_vector() {
		int m_len = move_data.size();
		fvec_t vec = {
			(float)active,
			(float)(curr_condition),
			(float)(best_condition),
			(float)(lvl),
			(float)(attack),
			(float)(defense),
			(float)(special_attack),
			(float)(special_defense),
			(float)(speed),
			(float)(is_trapped),
			(float)(is_fainted),
		};

		for (auto each : move_data) {
			vec.insert(vec.end(), each.as_vector().begin(), each.as_vector().end());
		}

		// is_active, if move 1, move 1 data, if move x, move x data, 
		return vec;
	}
};
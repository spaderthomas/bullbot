#pragma once
#include <vector>
#include <string>
#include <array>
#include "Definitions.hpp"
#include "EnvironmentSettings.hpp"

template<typename T>
struct HasDataRepresentation {
	virtual std::vector<T> as_vector() = 0;
};

struct HasFloatDataRepresentation : HasDataRepresentation<float> {};

struct MoveData : HasFloatDataRepresentation {
	std::string name;
	short pp = 16;
	// attributes not passed directly
	bool disabled = false;

	fvec_t as_vector() {
		fvec_t data;
		data.push_back((float)pp);

		// add movedex data
		if (movedex.count(name)) {
			auto& mdata = movedex.at(name);
			data.insert(data.end(), mdata.begin(), mdata.end());
		} else {
			fvec_t empty_vec(8, -1);
			data.insert(data.end(), empty_vec.begin(), empty_vec.end());
		}
		return data;
	}
};


struct PokemonData : HasFloatDataRepresentation {
	// attributes passed
	bool active = false;
	short curr_condition = 0;
	short lvl = 0;
	std::array<MoveData, 4> move_data;

	// attributes not passed directly
	std::string name = "";
	std::string ident = "";
	std::string item = "";
	std::string pokeball = "";
	std::string base_ability = "";
	bool is_trapped = false;
	bool is_fainted = false;

	fvec_t as_vector() {
		int m_len = move_data.size();
		fvec_t data = {
			(float)active,
			(float)(curr_condition),
			(float)(lvl)
		};
		if (pokedex.count(ident)) {
			auto& mdata = movedex.at(name);
			data.insert(data.end(), mdata.begin(), mdata.end());
		} else {
			fvec_t empty_vec(6, -1);
			data.insert(data.end(), empty_vec.begin(), empty_vec.end());
		}

		for (auto each : move_data) {
			auto e = each.as_vector();
			data.insert(data.end(), e.begin(), e.end());
		}

		return data;
	}
};

struct EnvironmentData : HasFloatDataRepresentation {
	bool game_over = false;
	// the maximum number of moves a pokemon can have is 4
	// the maximum number of pokemon a player can *switch* to is 5 pokemon; the player can only have up to 5 non-active pokemon after the initial swutcg
	// kept as strings to be friendly with other implementations
	std::string player_id;
	std::array<PokemonData, 6> player_team;
	std::array<PokemonData, 6> opponent_team;
	action_arr_t get_available_actions() {

	}

	fvec_t as_vector() {
		fvec_t data;
		for (auto each: player_team) {
			auto evec = each.as_vector();
			data.insert(data.end(), evec.begin(), evec.end());
		}
		for (auto each : opponent_team) {
			auto evec = each.as_vector();
			data.insert(data.end(), evec.begin(), evec.end());
		}
		return data;
	}
};

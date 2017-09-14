/*
Header containing room data

*/
#pragma once
#include <vector>
#include <string>
#include "PokemonData.hpp"
struct Room {};
struct BattleRoom : Room {
	std::string title;
	std::string rq_id;
	std::vector<PokemonData> teamdata;
};
struct ChatRoom : Room {};
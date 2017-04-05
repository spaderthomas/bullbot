#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stdlib.h> 
#include <time.h> 
//wherever that is...
#include "Pokemon.h"

struct PlayerMove
{
	bool isSwitch;
	Pokemon *pokemon = NULL;
	std::string moveName = "";
};

class Player
{
public:
	Player(std::string filepath,
	 std::unordered_map<string, Pokemon> team,
	 std::unordered_map<string, Pokemon> knownOpponentTeam,
	 std::unordered_map<string, Pokemon> opponentTeam);
	Player(std::string filepath);
	void setCurrentOut(PlayerMove move);
	void setOpponentCurrentOut(PlayerMove move);
	void processTurn(yourMove, opponentMove);
	PlayerMove move();

	bool getHasWon() {
		return hasWon;
	}

private:
	unordered_map<string, Pokemon> team;
	unordered_map<string, Pokemon> knownOpponentTeam;
	unordered_map<string, Pokemon> opponentTeam;
	bool hasWon;
	std::string current_out;
	std::string opponent_current_out;
	void updateOpponentTeam(PlayerMove move);
	void buildTeam(std::string filepath);
};

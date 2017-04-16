#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stdlib.h> 
#include <time.h> 
#include "Pokemon.h"

struct PlayerMove
{
    bool isSwitch;
    Pokemon *pokemon = NULL;
    std::string moveName = "";
    int damage = 0;
};

class Player
{
public:
	Player(std::string filepath);
        Pokemon getCurrentOut();
        Pokemon getPokemon(std::string name);
	void setCurrentOut(PlayerMove move);
	void processTurn(PlayerMove yourMove, PlayerMove opponentMove);
        bool isWinner();
	PlayerMove move();
private:
	std::unordered_map<std::string, Pokemon> team;
	bool hasWon;
	std::string currentOut;
	void buildTeam(std::string filepath);
};

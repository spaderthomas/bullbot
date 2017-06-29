#pragma once

#include "Pokemon.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unordered_map>

struct PlayerMove {
  bool isSwitch;
  Pokemon *pokemon = NULL;
  std::string moveName = "";
  int damage = 0;
  bool isSuccess = true;
};

bool compMoves(PlayerMove *p1Move, PlayerMove *p2Move);

class Player {
public:
  Player(std::string filepath);
  Pokemon getCurrentOut();
  Pokemon getPokemon(std::string name);
  void setCurrentOut(PlayerMove move);
  void processTurn(PlayerMove yourMove, PlayerMove opponentMove);
  bool isWinner();
  PlayerMove move();
  PlayerMove makeSwitchOnFaint();
private:
  std::unordered_map<std::string, Pokemon> team;
  bool hasWon;
  std::string currentOut;
  void buildTeam(std::string filepath);
};

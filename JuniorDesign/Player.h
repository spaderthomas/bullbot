#pragma once

#include "Pokemon.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <unordered_map>

class Player {
public:
  Player(std::string filepath, int id, PlayerMove (*moveFunc)(Player*));
  int id;
  Pokemon *getCurrentOut();
  Pokemon getPokemon(std::string name);
  void setCurrentOut(PlayerMove move);
  void processTurn(PlayerMove yourMove, PlayerMove opponentMove);
  PlayerMove move();
  PlayerMove randomMove(Player *p);
  PlayerMove makeSwitchOnFaint();
  int numAlive;
  bool hasLost;
  PlayerMove (*moveFunc)(Player*);
  std::unordered_map<std::string, Pokemon> team;
  std::string currentOut;
  void buildTeam(std::string filepath);
};

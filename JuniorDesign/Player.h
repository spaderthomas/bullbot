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

  bool operator<( const PlayerMove& p2Move ) const { 
    int p1speed = pokemon->getStats()["speed"];
    int p2speed = p2Move.pokemon->getStats()["speed"];

    if (p1speed == p2speed) {
      int random = rand() % 2;
      if ((random % 2) == 1) {
        return true;
      } else {
        return false;
      }
    }

    return p1speed > p2speed; 
  }      
};

class Player {
public:
  Player(std::string filepath);
  Pokemon getCurrentOut();
  Pokemon getPokemon(std::string name);
  void setCurrentOut(PlayerMove move);
  bool processTurn(PlayerMove yourMove, PlayerMove opponentMove);
  bool isWinner();
  PlayerMove move();

private:
  std::unordered_map<std::string, Pokemon> team;
  bool hasWon;
  std::string currentOut;
  void buildTeam(std::string filepath);
};

bool compareMoves(PlayerMove p1Move, PlayerMove p2Move);


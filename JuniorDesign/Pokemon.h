#pragma once

#include "libs/jsoncons/json.hpp"
#include "libs/jsoncons_ext/jsonpath/json_query.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class Pokemon {
public:
  Pokemon(std::string n = "");
  std::string getName();
  std::vector<std::string> getTypes();
  std::vector<std::string> getMoves();
  std::string getMove(int i);
  std::unordered_map<std::string, int> getStats();
  void addMove(std::string move);
  bool isFainted();
  void setFainted(bool status);
  bool containsMove(std::string moveName);
  int getHP();
  void takeDamage(int damage);

private:
  std::string name;
  std::vector<std::string> types;
  std::vector<std::string> moves;
  std::unordered_map<std::string, int> stats;
  int hp = 0;
  bool isBurned = false;
  bool isParalyzed = false;
  bool isAsleep = false;
  bool isFrozen = false;
  bool fainted = false;
};

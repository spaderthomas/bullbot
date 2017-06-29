#include "Pokemon.h"

Pokemon::Pokemon(std::string n) {
  this->name = n;

  // reading from json file
  std::ifstream is("pokemon.json");
  jsoncons::json pokemon;
  is >> pokemon;

  // grab hp, types, stats
  this->hp = pokemon[name]["stats"]["hp"].as<int>();

  for (const auto &type : pokemon[name]["type"].array_range()) {
    this->types.push_back(type.as<std::string>());
  }

  for (const auto &stat : pokemon[name]["stats"].object_range()) {
    stats[stat.key()] = stat.value().as<int>();
  }
}

std::string Pokemon::getName() { return this->name; }

int Pokemon::getHP() { return this->hp; }

// returns the vector with the values for types
// but prints them out first
std::vector<std::string> Pokemon::getTypes() {
  // for (int i = 0; i < types.size(); i++) {
  //     std::cout << types[i];
  //     if (i < types.size() - 1) {
  //         std::cout << ',' << '\n';
  //     }
  // }
  // std::cout << '\n';
  return this->types;
}

// returns the vector with the values for moves
// but prints them out first
std::vector<std::string> Pokemon::getMoves() {
  for (int i = 0; i < this->moves.size(); i++) {
    std::cout << this->moves[i];
    if (i < this->moves.size() - 1) {
      std::cout << ',' << '\n';
    }
  }
  std::cout << '\n';
  return this->moves;
}
std::string Pokemon::getMove(int i) { return this->moves[i]; }

bool Pokemon::containsMove(std::string moveName) {
  for (int i = 0; i < this->moves.size(); i++) {
    if (moves[i] == moveName)
      return true;
  }
  return false;
}

// returns the vector with the values for stats
std::unordered_map<std::string, int> Pokemon::getStats() { return this->stats; }

// adds move to move vector
void Pokemon::addMove(std::string move) {
  this->moves.push_back(move);
  ;
}

// returns if pokemon is fainted
bool Pokemon::isFainted() { return this->hp <= 0; }

void Pokemon::setFainted(bool status) { this->fainted = status; }

void Pokemon::takeDamage(int damage) {
  this->hp = this->hp - damage > 0 ? this->hp - damage : 0;
}

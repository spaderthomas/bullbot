#pragma once

// Defines structs used to pass data from PS to learners
struct PokemonData {
  int id;  // from [1:151], indicates which pokemon
  int types[2];
  int moves[4];
  int stats[5];
  int hp;
  bool burned = false;
  bool paralyzed = false;
  bool asleep = false;
  bool frozen = false;
  bool fainted = false;
  bool active;
};
  
struct Turn {
  PokemonData player[6];
  PokemonData opponent[6];
};

struct {
  json moveData;
  json pokemonData;
  json typeData;

  void initGameData() {
	  std::ifstream moveDataStream;
    moveDataStream.open("move-data.json");
    moveDataStream >> moveData;

    std::ifstream pokemonDataStream;
    pokemonDataStream.open("pokemon.json");
    pokemonDataStream >> pokemonData;

    std::ifstream typeDataStream;
    typeDataStream.open("type-chart.json");
    typeDataStream >> typeData;
  }
} globalGameData;

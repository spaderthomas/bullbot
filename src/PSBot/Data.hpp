// Holds global data which define Pokemon, moves, and the type chart
struct {
  json moveData;
  json pokemonData;
  json typeData;

  void initGameData() {
	  std::ifstream moveDataStream;
    moveDataStream.open("data/move-data.json");
    moveDataStream >> moveData;

    std::ifstream pokemonDataStream;
    pokemonDataStream.open("data/pokemon.json");
    pokemonDataStream >> pokemonData;

    std::ifstream typeDataStream;
    typeDataStream.open("data/type-chart.json");
    typeDataStream >> typeData;
  }
} globalGameData;

std::string getTypeNameFromID(int id) {
  auto& typeData = globalGameData.typeData;
  for (json::iterator it = typeData.begin(); it != typeData.end(); ++it) {
    if (it.value()["index"] == id) {
      return it.key();
    }
  }

  return ""; 
}

int getTypeIDFromName(std::string name) {
  return globalGameData.typeData[name]["index"];
}

// Data to represent instantiations of Pokemon in game
struct MoveData {
  int id;
  short pp = 16;
	std::string name;

	fvec_t asVector() {
		fvec_t data = {
      (float)id,
      (float)pp
    };
		return data;
	}

  void initFromName(std::string moveName) {
    id = globalGameData.moveData[moveName]["index"];
    name = moveName;
  }
};


struct PokemonData {
  int id;
  int hp;

  // atk, def, spatk, spdef, speed
  std::unordered_map<std::string, int> stats;
  std::vector<std::string> types;
  std::vector<MoveData> moves;
  std::unordered_map<std::string, int> boosts;
  unsigned int level = 0;

  // one bit for each minor status
  uint8_t burned = 0x0;
  uint8_t paralyzed = 0x0;
  uint8_t asleep = 0x0;
  uint8_t frozen = 0x0;

  bool fainted = false;
	bool active = false;
  bool trapped = false;

  std::string name;

	fvec_t asVector() {
    // Add all our basic float values
		fvec_t data = {
      (float) id,
      (float) hp,
      (float) (burned | paralyzed | asleep | frozen),
      (float) fainted,
      (float) active,
      (float) trapped
		};

    // Add all types, with -1 for single-typed
    int typesRemaining = 2;
    fox_for(indxType, types.size()) {
      data.push_back((float) getTypeIDFromName(types[indxType]));
      typesRemaining--;
    }
    fox_for(indxType, typesRemaining) {
      data.push_back((float) -1);
    }
    
    // Add all moves, with -50 for unknown
    int movesRemaining = 4;
    fox_for(indxMove, moves.size()) {
      fvec_t moveVec = moves[indxMove].asVector();
      data.insert(data.begin(), moveVec.begin(), moveVec.end());
      movesRemaining--;
    }
    fox_for(indxMove, movesRemaining) {
      data.push_back((float) -50);
    }

    // Add all boosts
    vector<string> statsNames = {"atk", "def", "spa", "spd", "spe"};
    for (auto& stat : statsNames) {
      auto it = boosts.find(stat);
      if (it != boosts.end()) {
        data.push_back(it->second);
      } else {
        data.push_back(0);
      }
    }

    // Add all stats
    data.push_back((float) stats["atk"]);
    data.push_back((float) stats["def"]);
    data.push_back((float) stats["spatk"]);
    data.push_back((float) stats["spdef"]);
    data.push_back((float) stats["speed"]);

		return data;
	}

  int hasMove(std::string moveName) {
    fox_for(indxMove, moves.size()) {
      if (moves[indxMove].name == moveName) {
        return indxMove;
      }
    }

    return -1;
  }
};
typedef std::vector<PokemonData> team_t;

void initPokemonFromName(PokemonData &newPokemon, std::string name) {
  // ID
  newPokemon.name = name;
  newPokemon.id = globalGameData.pokemonData[name]["index"];

  // Types
  std::vector<std::string> types = globalGameData.pokemonData[name]["type"];
  newPokemon.types = types;

  // Stats
  newPokemon.hp = globalGameData.pokemonData[name]["stats"]["hp"];
  std::unordered_map<std::string, int> stats;
  stats["atk"] = globalGameData.pokemonData[name]["stats"]["attack"];
  stats["def"] = globalGameData.pokemonData[name]["stats"]["defense"];
  stats["spatk"] = globalGameData.pokemonData[name]["stats"]["special"];
  stats["spdef"] = globalGameData.pokemonData[name]["stats"]["special"];
  stats["speed"] = globalGameData.pokemonData[name]["stats"]["speed"];
  newPokemon.stats = stats;

  // Level
  newPokemon.level = 100; 
}

fvec_t stateAsVector(team_t& playerTeam, team_t& opponentTeam) {
		fvec_t data;

    // Append player's Pokemon (which are always all known)
		for (auto pokemon : playerTeam) {
			fvec_t pokemonVec = pokemon.asVector();
			data.insert(data.end(), pokemonVec.begin(), pokemonVec.end());
		}

    // Append opponent's Pokemon, filling in -1s for unknown information
    for (auto pokemon : opponentTeam) {
			fvec_t pokemonVec = pokemon.asVector();
			data.insert(data.end(), pokemonVec.begin(), pokemonVec.end());
		}
    
		return data;
}
 
PokemonData* getPokemon(std::string pokemonName, team_t& team) {
  fox_for(indxPkmn, team.size()) {
    if (team[indxPkmn].name == pokemonName) {
      return &team[indxPkmn];
    }
  }

  return nullptr;
}

PokemonData* getActivePokemon(team_t& team) {
  fox_for(indxPkmn, team.size()) {
    if (team[indxPkmn].active) {
      return &team[indxPkmn];
    }
  }
  
  return nullptr;
}

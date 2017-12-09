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

  static std::string getTypeNameFromID(int id) {
    for (json::iterator it = typeData.begin(); it != typeData.end(); ++it) {
      if (it.value()["index"] == id) {
        return it.key();
      }
    }
  }

  static int getTypeIDFromName(std::string name) {
    return typeData[name]["index"];
  }
} globalGameData;

struct MoveData {
  int id;
  short pp = 16;
	std::string name;

	fvec_t as_vector() {
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
  std::vector<std::string> types;
  int hp;
  std::unordered_map<std::string, int> stats;
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

	fvec_t as_vector() {
		fvec_t data = {
      (float) id,
			(float) level,
      (float) hp,
      (float) (burned | paralyzed | asleep | frozen),
      (float) fainted,
      (float) active,
      (float) trapped
		};

    fox_for(indxType, 2) {
      data.push_back((float) getTypeIDFromName(types[indxType]));
    }
    fox_for(indxMove, 4) {
      fvec_t moveVec = moves[indxMove].as_vector();
      data.insert(data.begin(), moveVec.begin(), moveVec.end());
    }
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

fvec_t Data::stateAsVector(std::vector<PokemonData>& playerTeam, std::vector<PokemonData>& playerTeam) {
		fvec_t data;
		for (auto pokemon : playerTeam) {
			fvec_t pokemonVec = pokemon.as_vector();
			data.insert(data.end(), pokemonVec.begin(), pokemonVec.end());
		}
    for (auto pokemon : opponentTeam) {
			fvec_t pokemonVec = pokemon.as_vector();
			data.insert(data.end(), pokemonVec.begin(), pokemonVec.end());
		}
    
		return data;
}

PokemonData& Data::getPokemon(std::string pokemonName, std::vector<PokemonData> &team) {
  fox_for(indxPkmn, team.size()) {
    if (team[indxPkmn].name == pokemonName) {
      return team[indxPkmn];
    }
  }

  return nullptr;
}

PokemonData& Data::getActivePokemon(std::vector<PokemonData> &team) {
  fox_for(indxPkmn, team.size()) {
    if (team[indxPkmn].active) {
      return team[indxPkmn];
    }
  }

  return nullptr;
}

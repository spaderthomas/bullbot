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
  int types[2];
  std::vector<MoveData> moves;
  int stats[5] = {};
  std::unordered_map<std::string, int> boosts;
  unsigned int level = 0;
  int hp;

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
      data.push_back((float) types[indxType]);
    }
    fox_for(indxMove, 4) {
      fvec_t moveVec = moves[indxMove].as_vector();
      data.insert(data.begin(), moveVec.begin(), moveVec.end());
    }
    fox_for(indxStat, 5) {
      data.push_back((float) stats[indxStat]);
    }

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

struct GameState {
	std::vector<PokemonData> playerTeam;
	std::vector<PokemonData> opponentTeam;

	fvec_t as_vector() {
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
  
  PokemonData* getOpponentPokemon(std::string pokemonName) {
    fox_for(indxPkmn, opponentTeam.size()) {
      if (opponentTeam[indxPkmn].name == pokemonName) {
        return &opponentTeam[indxPkmn];
      }
    }

    return nullptr;
  }

  PokemonData* getPlayerPokemon(std::string pokemonName) {
    fox_for(indxPkmn, playerTeam.size()) {
      if (playerTeam[indxPkmn].name == pokemonName) {
        return &playerTeam[indxPkmn];
      }
    }

    return nullptr;
  }
};

struct PSBattleData {
  GameState state;
  std::string playerID;
  std::ofstream eventLog;
};

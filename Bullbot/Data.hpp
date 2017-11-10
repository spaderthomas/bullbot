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
};

struct PokemonData {
  int id;
  int types[2];
  MoveData moves[4];
  int stats[5];
  unsigned int level = 0;
  int hp;
  uint8_t burned; // one bit for each
  uint8_t paralyzed;
  uint8_t asleep;
  uint8_t frozen;
  bool fainted;
	bool active = false;

  std::string name;

	fvec_t as_vector() {
		fvec_t data = {
      (float) id,
			(float) level,
      (float) hp,
      (float) (burned | paralyzed | asleep | frozen),
      (float) fainted,
      (float) active
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
};

struct GameState {
	PokemonData playerTeam[6];
	PokemonData opponentTeam[6];

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
};

#pragma once

template<typename T>
struct HasDataRepresentation {
	virtual std::vector<T> as_vector() = 0;
};

struct HasFloatDataRepresentation : HasDataRepresentation<float> {};

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

struct MoveData : HasFloatDataRepresentation {
	std::string name;
	short pp = 16;
	// attributes not passed directly
	bool disabled = false;

	fvec_t as_vector() {
		fvec_t data;
		data.push_back((float)pp);

		// add movedex data
		if (movedex.count(name)) {
			auto& mdata = movedex.at(name);
			data.insert(data.end(), mdata.begin(), mdata.end());
		} else {
			//std::cout << "couldn't find move " << name.c_str() << std::endl;
			fvec_t empty_vec(8, -1);
			data.insert(data.end(), empty_vec.begin(), empty_vec.end());
		}
		return data;
	}
};


struct PokemonData : HasFloatDataRepresentation {
  int id;
  int types[2];
  int moves[4];
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

		data.insert(data.end(), std::begin(types), std::end(types));
		data.insert(data.end(), std::begin(moves), std::end(moves));
		data.insert(data.end(), std::begin(stats), std::end(stats));

		return data;
	}
};

struct EnvironmentData : HasFloatDataRepresentation {
	bool game_over = false;
	std::string player_id;
	PokemonData player_team[6];
	PokemonData opponent_team[6];

	fvec_t as_vector() {
		fvec_t data;
		for (auto each: player_team) {
			auto evec = each.as_vector();
			data.insert(data.end(), evec.begin(), evec.end());
		}
		for (auto each : opponent_team) {
			auto evec = each.as_vector();
			data.insert(data.end(), evec.begin(), evec.end());
		}
		return data;
	}
};

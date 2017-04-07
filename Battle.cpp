//#include "Battle.h"
/*void Party::make_move(Battle& battle) {
	if (policy_function != nullptr) {
		policy_function(*this, battle);
	}
	else {
		std::vector<std::vector<float>> options;
		if (active_pokemon != nullptr) {
			for (auto option : active_pokemon->moveset) {
				std::vector<float> option_attributes;
				std::vector<float> temp_pokemon_attributes = active_pokemon->as_attributes();

				option_attributes.push_back(0);
				option_attributes.insert(option_attributes.end(), temp_pokemon_attributes.begin(), temp_pokemon_attributes.end());
				options.push_back(option_attributes);

				std::printf("[");
				for (auto each : active_pokemon->as_attributes()) {
					std::printf("%f ", each);
				}
				std::printf("]\n");
				std::printf("Available option: move   - %s\n", option.name.c_str());
			}
		}
		for (int i = 0; i < pokemon.size(); ++i) {
			Pokemon* switch_pokemon = &pokemon[i];
			if (switch_pokemon != active_pokemon) {
				std::vector<float> option_attributes;
				std::vector<float> temp_pokemon_attributes = active_pokemon->as_attributes();
				std::vector<float> switch_pokemon_attributes = switch_pokemon->as_attributes();


				option_attributes.push_back(1);
				option_attributes.insert(option_attributes.end(), temp_pokemon_attributes.begin(), temp_pokemon_attributes.end());
				Pokemon* opposition = battle.get_opposing_party(this)->active_pokemon;

				option_attributes.insert(option_attributes.end(), switch_pokemon_attributes.begin(), switch_pokemon_attributes.end());

				options.push_back(option_attributes);

				std::printf("Available option: switch - %s\n", switch_pokemon->name.c_str());
			}

		}
		std::printf("skipped because %s has no policy\n", name.c_str());
	}
};*/



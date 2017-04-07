#pragma once
#include <vector>
#include "Pokemon.h"
#include "Move.h"
#include "Action.h"
#include <random>
#include <memory>

std::default_random_engine rng;
struct Party {
	std::string name = "Trainer";//for convenience
	std::vector<Pokemon> pokemon;
	Pokemon *active_pokemon = nullptr;
	Action chosen_action = Action();

	void choose_action(Party* opposition) {
		std::vector<Action> actions = get_actions(opposition);
		std::vector<Action> valid_actions;
		for (auto action : actions) {
			if (action.valid) {
				valid_actions.push_back(action);
			}
		}
		chosen_action = Action();
		int index;
		if (valid_actions.size() != 0) {
			std::uniform_int_distribution<int> distribution(0, (int)valid_actions.size() - 1);
			index = distribution(rng);
			chosen_action = valid_actions[index];
			std::printf("chosen action %d %d\n", index, valid_actions.size());
		}
		if (index > pokemon.size()) {
			//todo: make this correct for struggle
			chosen_action.move = active_pokemon->moveset[index - pokemon.size];
		}
	}

	std::vector<Action> get_actions(Party* opposition) {
		std::vector<Action> actions;
		for (auto& poke : pokemon) {
			auto action = Action(Action::ActionType::RECALL, active_pokemon, &poke, Move());
			action.valid = (!poke.active && !poke.fainted) ? true : false;
			actions.push_back(action);
		}
		while (actions.size() < 6) {
			auto action = Action(Action::ActionType::RECALL, nullptr , nullptr, Move());
			action.valid = false;
			actions.push_back(action);
		}
		if (active_pokemon != nullptr) {
			for (Move move : active_pokemon->moveset) {
				Action action = Action(Action::ActionType::MOVE, active_pokemon, opposition->active_pokemon, move);
				action.valid = ((move.pp > 0)) ? true : false;
				actions.push_back(action);
			}
		}
		while (actions.size() < 11) {
			auto action = Action(Action::ActionType::MOVE, nullptr, nullptr, Move());
			action.valid = false;
			actions.push_back(action);
		}
		return actions;
	}
};
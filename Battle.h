#pragma once
#include <random>
#include <vector>
#include "Type.h"
#include "Move.h"
#include "Party.h"
#include "CircularLinkedList.h"

struct Battle {
	CircularLinkedList<Party> parties;
	void conduct_turn() {
		auto curr_party = parties.get_head();
		//could later change to while and count
		for (int i = 0; i < parties.size; ++i) {
			Party& party_data = curr_party->get_data();
			std::printf("Party %s\n", party_data.name.c_str());
			party_data.choose_action(&curr_party->get_next()->data);
			if (party_data.chosen_action.action_type == Action::ActionType::MOVE) {
				if (party_data.chosen_action.move.isvalid) {
					std::printf("Party %s is executing move %d\n", party_data.name.c_str(), party_data.chosen_action.move.power);
				}
			} else {

				std::printf("Party action %d\n", party_data.chosen_action.action_type);
			}
			curr_party = curr_party->get_next();
		}


	}
	void pass_results() {}
	//quick and dirty, needs revision
}; 



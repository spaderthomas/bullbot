#pragma once
#include "Move.h"
struct Action {
	enum ActionType {
		RECALL,
		MOVE
	} action_type;
	Action() {};
	Action(ActionType action_type, Pokemon* user, Pokemon* target, Move move) : action_type(action_type), user(user), target(target), move(move) {}

	bool valid = false;
	Pokemon* user = nullptr;
	Pokemon* target = nullptr;
	Move move;
};
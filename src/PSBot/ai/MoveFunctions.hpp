int RandomAICallback(team_t& playerTeam, team_t& opponentTeam) {
  action_arr_t availableActions = getAvailableActions(state.playerTeam);
  if (!available_actions->size()) {
    return -1;
  }
  std::random_device randSeed;
	std::mt19937 engine{randSeed()};
  std::uniform_int_distribution<int> rng(0, available_actions->size() - 1);
  return (*available_actions)[rng(engine)];
}

int runRandomSimulations(team_t& playerTeam, team_t& opponentTeam) {
  Simulation sim;
  sim.playerMoveFunc = &RandomAICallback;
  sim.opponentMoveFunc = &RandomAICallback;
  sim.

  int wins = 0;
  fox_for(indxSim, DEFAULT_NUM_SIMULATIONS) {
    wins += simulate(player);
  }
}


// MCTS

// Deep Q

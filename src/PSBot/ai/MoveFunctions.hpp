int randomChoice(team_t& playerTeam, team_t& opponentTeam, action_arr_t& availableActions) {
  if (!availableActions.size()) {
    return -1;
  }
  std::random_device randSeed;
	std::mt19937 engine{randSeed()};
  std::uniform_int_distribution<int> rng(0, availableActions.size() - 1);
  return availableActions[rng(engine)];
}

int runRandomSimulations(team_t& playerTeam, team_t& opponentTeam, action_arr_t availableActions) {
  SimulationInfo sim;
  sim.playerMoveFunc = &randomChoice;
  sim.oppMoveFunc = &randomChoice;
  sim.playerTeam = playerTeam;
  sim.oppTeam = opponentTeam;
  sim.battleOver = false;

  int wins = 0;
  fox_for(indxSim, DEFAULT_NUM_SIMULATIONS) {
    wins += simulate(sim);
  }

  return 0;
}


// MCTS

// Deep Q

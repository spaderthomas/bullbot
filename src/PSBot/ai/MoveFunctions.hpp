int randomChoice(team_t& playerTeam, team_t& opponentTeam, action_arr_t& availableActions) {
  if (!availableActions.size()) {
    return -1;
  }
  std::random_device randSeed;
	std::mt19937 engine{randSeed()};
  std::uniform_int_distribution<int> rng(0, availableActions.size() - 1);
  return availableActions[rng(engine)];
}

int runRandomSimulations(team_t& playerTeam, team_t& opponentTeam, action_arr_t& availableActions) {
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

void build_td_learner() {
  MLP<float, float, float> net(312, 9);
  
  Layer<float> input_layer = Layer<float>(312, TanhActivation<float>());
  Layer<float> first_hidden_layer = Layer<float>(500, TanhActivation<float>());
  Layer<float> output_layer = Layer<float>(9, LinearActivation<float>());
  
  net.append_layer(input_layer);
  net.append_layer(first_hidden_layer);
  net.append_layer(output_layer);
  net.finalize(true);

  learner.setValueEstimator(net);
  learner.q_estimator->learning_rate = .4;
	learner.q_estimator->learning_decay = 1.0/10000;

  auto training_iter = 10000;
	auto training_div = training_iter / 100;

  for (int i = 0; i < training_iter; ++i) {
    vector<float> sample(312, 1);
    vector<float> reward = {1.f};
    vector<size_t> actions = {0};
    
    learner.update_values(sample, reward, actions);
  }
}

// Teach it to distinguish pos and neg
void build_test_net() {
  MLP<float, float, float> net(1, 2);
  
  Layer<float> input_layer = Layer<float>(1, TanhActivation<float>());
  Layer<float> first_hidden_layer = Layer<float>(5, TanhActivation<float>());
  Layer<float> output_layer = Layer<float>(2, LinearActivation<float>());
  net.append_layer(input_layer);
  net.append_layer(first_hidden_layer);
  net.append_layer(output_layer);
  net.finalize(true);

  TDLearner<MLP<float, float, float>> test_learner(net);
  
  auto training_iters = 100;
	auto training_div = 50;

  for (int iter = 0; iter < training_iters; iter++) {
    float sample = iter % 2 ? iter : -1 * iter;
    vector<float> inputState = {sample};
    
    // Calculate output with highest estimated reward
    float max = -1000000.69;
    int bestAction = -1;
    fox_for(ioutput, net.num_outputs) {
      vector<float> outputVal = test_learner.get_value(inputState);
      if (outputVal[0] > max){
        max = outputVal[0];
        bestAction = ioutput;
      }
    }
    
    vector<float> trueReward;
    if (bestAction = 0) {
      trueReward = {-1.f * iter};
    } else {
      trueReward = {(float)iter};
    }
    
    vector<size_t> actions = {(size_t)bestAction};
    
    learner.update_values(inputState, trueReward, actions);
  }

  vector<float> posSample = {10};
  vector<float> posReward = {test_learner.get_value(posSample)};
  vector<float> negSample = {-10};
  vector<float> negReward = {test_learner.get_value(posSample)};

}

// Deep Q

// MCTS

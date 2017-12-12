
// Holds game state along with a move function for each player to simulate with
struct SimulationInfo {
  action_callback_t playerMoveFunc;
  action_callback_t oppMoveFunc;
  team_t playerTeam;
  team_t opponentTeam;
  bool battleOver;
};
  
int calcDamage(MoveData& move, PokemonData& attacker, PokemonData& defender) {
  std::string moveType = globalGameData.moveData["type"];
  json moveChart = typeData[moveType];
  int random = (rand() % 38) + 217;
  int pow = globalGameData.moveData[move.name]["power"];

  if (pow == 0) {
    return 0;
  }
  // Get type effectiveness multiplier
  float mult = 1.0;
  for (const auto& type : defender.types) {
    vector<string> immune = moveChart["immune"];
    vector<string> notEffective = moveChart["nve"];
    vector<string> superEffective = moveChart["s-e"];
    fox_for(indxImmune, immune.size()) {
      if (immune[indxImmune] == type) {
        mult = 0.f;
      }
    }
    fox_for(indxNVE, notEffective.size()) {
      if (notEffective[indxNVE] == type) {
        mult *= .5;
      }
    }
    fox_for(indxSE, superEffective.size()) {
      if (superEffective[indxSE] == type) {
        mult *= 2;
      }
    }
  }

  // Check if move's type matches any attacker type (STAB)
  for (auto& type : attacker.types()) {
    if (globalGameData.moveData[move.name]["type"] == type) {
      mult *= 1.5;
    }
  }

  // Get final damage from damage formula (macro defined in Main.cpp)
  return floor(DAMAGE(attack, defense, pow, random, mult));
}

int simulate(SimulationInfo sim) {
  int playerMove;
  int opponentMove;
  team_t& playerTeam = sim.playerTeam;
  team_t& opponentTeam = sim.opponentTeam;

  while (!sim.battleOver) {
    playerMove = sim.playerMoveFunc(playerTeam, opponentTeam);
    opponentMove = sim.oppMoveFunc(opponentTeam, playerTeam);

    // Decide which moves goes first. 0 means player, 1 means opponent
    // Order only matters if both players attack
    int first;
    if (isSwitch(playerMove)) {
      first = 0;
    } else if (isSwitch(opponentMove)) {
      first = 1;
    } else {
      int playerSpeed = Data::getActivePokemon(playerTeam).stats["speed"];
      int opponentSpeed = Data::getActivePokemon(opponentTeam).stats["speed"]; 

      if (playerSpeed == opponentSpeed) { // handle speed ties
        std::random_device randSeed;
        std::mt19937 engine{randSeed()};
        std::uniform_int_distribution<int> rng(0, 1);
        first = rng(engine);
      } else {
        first = playerSpeed < opponentSpeed;
      }
    }
    int order[2] = {first, !first};

    // Simulate the moves in order
    fox_for(indxMove, 2) {
      if (order[indxMove] == 0) {
        int activeMove = playerMove;
        int inactiveMove = opponentMove;
        team_t& activePlayerTeam = playerTeam;
        team_t& inactivePlayerTeam = opponentTeam;
      } else {
        int activeMove = opponentMove;
        int inactiveMove = playerMove;
        team_t& activePlayerTeam = opponentTeam;
        team_t& inactivePlayerTeam = playerTeam;
      }
      
      if (isSwitch(activeMove)) {
        PokemonData& oldActivePokemon = getActivePokemon(activePlayerTeam);
        PokemonData& newActivePokemon = activePlayerTeam[activeMove - 4];
        oldActivePokemon.active = false;
        newActivePokemon.active = true;
      } else {
        PokemonData& attackingPokemon = getActivePokemon(activePlayerTeam);
        PokemonData& defendingPokemon = getActivePokemon(inactivePlayerTeam);
        if (!attackingPokemon.fainted) {
          defendingPokemon.hp -= calcDamage(attackingPokemon.moves[activeMove], defendingPokemon);
          if (defendingPokemon.hp <= 0) {
            defendingPokemon.fainted = true;
          }
        }
      }
    }
    
  }

  int winner = 69;
  return winner;
}


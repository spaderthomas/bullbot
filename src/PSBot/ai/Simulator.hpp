#define isSwitch(move) move > 3

// Holds game state along with a move function for each player to simulate with
struct Simulation {
  action_callback_t playerMoveFunc;
  action_callback_t oppMoveFunc;
  team_t playerTeam;
  team_t opponentTeam;
  bool battleOver;
};

int calcDamage(MoveData& move, PokemonData& defender) {
  int random = (rand() % 38) + 217;
  int pow = globalGameData.moveData[move.name]["power"];
  std::string moveType = globalGameData.moveData["type"];
  json moveChart = typeData[moveType];
    
  // Get type effectiveness multiplier
  float mult = 1.0;
  for (const auto& type : defender.types) {
    vector<string> immune = moveChart["immune"];
    vector<string> notEffective = moveChart["nve"];
    vector<string> superEffective = moveChart["s-e"];
    for (int i = 0; i < immune.size(); i++) {
      if (immune[i] == type) {
        mult = 0.f;
      }
    }
    for (int i = 0; i < notEffective.size(); i++) {
      if (notEffective[i] == type) {
        mult *= .5;
      }
    }
    for (int i = 0; i < superEffective.size(); i++) {
      if (superEffective[i] == type) {
        mult *= 2;
      }
    }
  }

  // ---- LEFT OFF HERE ---- // 
  // check for stab
  for (int i = 0; i < attackPkmn.types.size(); i++) {
    if (moveType == attackPkmn.types[i]) {
      mult *= 1.5;
    }
  }

  // Get final damage
  int damage = pow == 0 ? 0 : floor(DAMAGE(attack, defense, pow, random, mult));

  /*
  printf("%s used %s against %s. The multiplier was %.3f. The base power was "
         "%i. The damage was %i.\n",
         attackMove.pokemon->getName().c_str(), attackMove.moveName.c_str(),
         defendMove.pokemon->getName().c_str(), mult, pow, damage);
  */
  return damage;
}

int simulate(Simulation& sim) {
  int playerMove;
  int opponentMove;
  team_t playerTeam = sim.playerTeam;
  team_t opponentTeam = sim.opponentTeam;

  while (!sim.battleOver) {
    playerMove = sim.playerMoveFunc(playerTeam, opponentTeam);
    opponentMove = sim.oppMoveFunc(opponentTeam, playerTeam);

    // Decide which moves goes first. 0 means player, 1 means opponent. Order only matters if both moves aren't switches.
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


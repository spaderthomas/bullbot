// Holds game state along with a move function for each player to simulate with
struct SimulationInfo {
  bool battleOver;
  team_t oppTeam;
  team_t playerTeam;
  action_callback_t oppMoveFunc;
  action_callback_t playerMoveFunc;
};

struct TurnInfo {
  int userMove;
  team_t& userTeam;
  action_arr_t validActions;

  TurnInfo(team_t& team) : userTeam(team) {}
};

int calcDamage(MoveData& move, PokemonData& attacker, PokemonData& defender) {
  int defense;
  int attack = attacker.stats["atk"];
  int random = (rand() % 38) + 217;
  int pow = globalGameData.moveData[move.name]["power"];
  std::string moveType = globalGameData.moveData["type"];
  std::string moveCategory = globalGameData.moveData[move.name]["category"];
  if (moveCategory == "special") {
    defense = defender.stats["spdef"];
  } else {
    defense = defender.stats["def"];
  }

  // Get implicated Pokemon's stats
  // Get type effectiveness multiplier
  float mult = 1.0;
  for (const auto& type : defender.types) {
    std::vector<std::string> immune = globalGameData.typeData[moveType]["immune"];
    std::vector<std::string> notEffective = globalGameData.typeData[moveType]["nve"];
    std::vector<std::string> superEffective = globalGameData.typeData[moveType]["s-e"];
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
  for (auto& type : attacker.types) {
    if (globalGameData.moveData[move.name]["type"] == type) {
      mult *= 1.5;
    }
  }

  // Get final damage from damage formula (macro defined in Main.cpp)|/utm |nidoranm|||doublekick|||||||
  return floor(DAMAGE(attack, defense, pow, random, mult));
}

int simulate(SimulationInfo sim) {
  while (!sim.battleOver) {
    // Generate a struct to hold some info about each player's turn
    TurnInfo playerTurn(sim.playerTeam);
    TurnInfo oppTurn(sim.oppTeam);

    // Push available actions for each user on this turn
    fox_for(id, 2) {
      TurnInfo& info = id ? oppTurn : playerTurn;
      action_arr_t availableActions;
      auto &team = info.userTeam;
      fox_for(indxPkmn, team.size()) {
        PokemonData &pokemon = team[indxPkmn];
        if (pokemon.active) {
          // Push back all moves
          availableActions.push_back(0);
          availableActions.push_back(1);
          availableActions.push_back(2);
          availableActions.push_back(3);
        } else {
          if (!pokemon.fainted) {
            // Push back all switches. 
            availableActions.push_back(indxPkmn + 4);
          }
        }
      }
      info.validActions = availableActions;
    }

    // Generate moves for the turn for each player
    playerTurn.userMove = sim.playerMoveFunc(playerTurn.userTeam, oppTurn.userTeam, playerTurn.validActions);
    oppTurn.userMove = sim.oppMoveFunc(oppTurn.userTeam, playerTurn.userTeam, oppTurn.validActions);

    // Decide which moves goes first. 0 means player, 1 means opponent
    // Order only matters if both players attack
    int first;
    if (isSwitch(playerTurn.userMove)) {
      first = PLAYER;
    } else if (isSwitch(oppTurn.userMove)) {
      first = OPPONENT;
    } else {
      int playerSpeed = getActivePokemon(playerTurn.userTeam)->stats["speed"];
      int oppSpeed = getActivePokemon(oppTurn.userTeam)->stats["speed"]; 

      if (playerSpeed == oppSpeed) { // handle speed ties
        std::random_device randSeed;
        std::mt19937 engine{randSeed()};
        std::uniform_int_distribution<int> rng(0, 1);
        first = rng(engine);
      } else {
        first = playerSpeed < oppSpeed;
      }
    }
    int order[2] = {first, !first};
 
    // Simulate the moves in order
    for (auto indxActive : order) {
      TurnInfo& active = (indxActive == PLAYER) ? playerTurn : oppTurn;
      TurnInfo& inactive = (indxActive == OPPONENT) ? playerTurn : oppTurn;

      // If move is switch, simply change which Pokemon is active
      if (isSwitch(active.userMove)) {
        getActivePokemon(active.userTeam)->active = false;
        active.userTeam[switchToIndx(active.userMove)].active = true;
      } else {
        PokemonData* attackingPokemon = getActivePokemon(active.userTeam);
        PokemonData* defendingPokemon = getActivePokemon(inactive.userTeam);
        if (!attackingPokemon->fainted) {
          defendingPokemon->hp -= calcDamage(attackingPokemon->moves[active.userMove], *attackingPokemon, *defendingPokemon);
          if (defendingPokemon->hp <= 0) {
            defendingPokemon->fainted = true;
          }
        }
      }
    }
    
  }

  int winner = PLAYER;
  return winner;
}


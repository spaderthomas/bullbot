void Battle::init() {
  this->players[0] = *Player::create("sample-team.txt", 0, &MCTS);
  this->players[1] = *Player::create("sample-team.txt", 1, &randomAttack);
}

int calcDamage(Player* attacker, Player* defender) {
  if (attacker->curMove.isSwitch) {
    return 0;
  }
  
  Pokemon attackPkmn = attacker->team[attacker->curMove.teamPos];
  Pokemon defendPkmn = defender->team[defender->curMove.teamPos];
  
  int attack = moves[attacker->curMove.moveName]["category"] == "physical"
                   ? attackPkmn.stats["attack"]
                   : attackPkmn.stats["special"];
  int defense = moves[attacker->curMove.moveName]["category"] == "physical"
                    ? defendPkmn.stats["defense"]
                    : defendPkmn.stats["special"];
  int pow = moves[attacker->curMove.moveName]["power"].as<int>();
  int random = (rand() % 38) + 217;

  // Get type effectiveness multiplier
  float mult = 1.0;
  std::vector<std::string> defenderTypes = defendPkmn.types;
  std::string moveType = moves[attacker->curMove.moveName]["type"].as<std::string>();

  // check type matchup for move used against defender types
  /*for (const auto &dtype : defenderTypes) {
	  for (auto it = types[moveType].object_range().begin();
		  it != types[moveType].object_range().end(); ++it) {
		  std::vector<std::string> thing =
			  it->value().as<std::vector<std::string>>();
		  for (int i = 0; i < thing.size(); i++) {
			  if (thing[i] == dtype) {
				  std::string effectiveness = it->key();
				  if (effectiveness == "immune") {
					  mult *= 0;
				  }
				  else if (effectiveness == "nve") {
					  mult *= .5;
				  }
				  else if (effectiveness == "s-e") {
					  mult *= 2;
				  }
			  }
		  }
	  }
  }*/

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

void Battle::battleLogic() {
  // Sort the moves. std::sort doesn't work here because random nature of
  // sorting causes undefined behavior
  int first;
  if (players[0].curMove.isSwitch) {
    first = 0;
  } else if (players[1].curMove.isSwitch) {
    first = 1;
  } else {
    int p1speed = players[0].team[players[0].curMove.teamPos].stats["speed"];
    int p2speed = players[1].team[players[1].curMove.teamPos].stats["speed"];

    if (p1speed == p2speed) { // Handle speed ties
      first = rand() % 2;
    } else {
      first = p1speed < p2speed;
    }
  }
  int order[2] = {first, !first};

  // Apply each players' moves
  for (auto &i : order) {
    Pokemon *active = &players[i].team[players[i].curMove.teamPos];
    Pokemon *inactive = &players[!i].team[players[!i].curMove.teamPos];

    if (players[i].curMove.isSwitch) {
      players[i].currentOut = players[i].curMove.teamPos;
    } else {
      if (!(active->fainted)) {
        inactive->hp -= calcDamage(&players[i], &players[!i]);
        if (inactive->hp <= 0) {
          inactive->fainted = true;
          /*std::cout << "Player " << players[!i].id << "'s "
                    << players[!i].team[players[!i].currentOut].name
                    << " fainted!\n";*/
        }
      }
    }
  }

  // Check for faints and send in new Pokemon
    for (int i = 0; i < 2; i++) {
      if (players[i].getCurrentOut()->fainted) {
        players[i].numAlive -= 1;
        if (players[i].numAlive == 0) {
          players[i].hasLost = true;
          return;
        }
        players[i].makeSwitchOnFaint();
        players[i].currentOut = players[i].curMove.teamPos;
      }
    }
}

// Loops a battle until someone wins
int Battle::battleLoop() {
  std::ifstream typestream("type-chart.json");
  typestream >> types;
  std::ifstream movestream("move-data.json");
  movestream >> moves;

  while (!players[0].hasLost && !players[1].hasLost) {
    /*printf("\n\nTurn %i:\n", turn);*/
    players[0].curMove = players[0].move(this);
    players[1].curMove = players[1].move(this);
    this->battleLogic();
	if (turn == 20) {
		players[0].moveFunc = &MCTS;
	}
    turn++;
    
    /*printf(
        "Player 1's %s has %i HP remaining.\nPlayer 2's %s has %i HP "
        "remaining.\n",
        players[0].getCurrentOut()->name.c_str(), players[0].getCurrentOut()->hp,
        players[1].getCurrentOut()->name.c_str(), players[1].getCurrentOut()->hp);
    printf("Player %i has %i Pokemon remaining!\n", players[0].id,
           players[0].numAlive);
    printf("Player %i has %i Pokemon remaining!\n", players[1].id,
           players[1].numAlive);*/
    // usleep(3000000);
  }

  // return the ID of the winning player
  return players[0].hasLost ? players[1].id : players[0].id;
}

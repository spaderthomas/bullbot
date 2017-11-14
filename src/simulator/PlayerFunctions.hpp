Player* Player::create(std::string filepath, int initID, moveFuncType initMoveFunc) {
  auto* p = new Player;
  p->hasLost = false;
  p->buildTeam(filepath);
  p->currentOut = 0;
  p->id = initID;
  p->numAlive = 6;
  p->moveFunc = initMoveFunc;
  return p;
}

/* Returns a pointer to the currently active Pokemon */
Pokemon* Player::getCurrentOut() {
  return &(this->team[currentOut]);
}

/* Handles setting currently active Pokemon by means of looking at the player's move */
void Player::setCurrentOut(PlayerMove move) {
  if (move.isSwitch)
    this->currentOut = move.teamPos;
}

Player::PlayerMove Player::move(Battle *battle) { return this->moveFunc(this, battle); }

/* Returns a PlayerMove representing a switch to a valid Pokemon */
void Player::makeSwitchOnFaint() {
  PlayerMove move;
  int oldPokemon = this->currentOut;
  int switchPokemon = this->currentOut;
  // Iterate randomly through valid Pokemon to switch.
  while (switchPokemon == oldPokemon || this->team[switchPokemon].fainted) {
    switchPokemon = rand() % 6;
  }
  move.isSwitch = true;
  move.teamPos = switchPokemon;
  /*std::cout << "Player " << this->id << " switched from " << oldPokemon
            << " to " << switchPokemon << " after a faint!" << std::endl;*/
  this->curMove = move;
}

/* Builds a team from a Pokemon Showdown formatted team string.
	The format of each individual pokemon is this:
	Jynx
	Blizzard
	Psychic
	Lovely Kiss
	Rest
	newline
	...
*/
void Player::buildTeam(std::string filepath) {
  std::ifstream infile(filepath);
  std::string line;
  std::string pokename;
  bool flagPokename = true; // Is the next line a pokemon name?
  int pos = 0;

  while (std::getline(infile, line)) {
    if (line.empty()) {
      flagPokename = true;
      pos++;
      continue;
    }

    if (flagPokename) {
      pokename = line;
      this->team[pos] = *Pokemon::create(pokename);
      flagPokename = false;
    } else {
      // Line is indicitive of a move
      this->team[pos].addMove(line);
    }
  }
}

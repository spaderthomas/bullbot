// Player function definitions
#include "Player.h"

Player::Player(std::string filepath) {
  // srand((unsigned)time(nullptr)); //TODO: there is almost definitely a better
  // place for this
  this->hasWon = false;
  this->buildTeam(filepath);
  this->currentOut = this->team.begin()->first;
}

/* Receives two moves that have had their damage calculated and their damage
field filled in. Updates which Pokemon is currently on the field. Updates damage
to current Pokemon as specified by the damage field of opponentMove.

This method returns true if a Player's active Pokemon faints as a result of the
opponent's move.

 */
bool Player::processTurn(PlayerMove yourMove, PlayerMove opponentMove) {
  this->setCurrentOut(yourMove);
  this->getCurrentOut().takeDamage(opponentMove.damage);
  if (yourMove.pokemon->getHP() <= 0) {
    yourMove.pokemon->setFainted(true);
    this->team.erase(yourMove.pokemon->getName());
    return true;
  }
  return false;
}

Pokemon Player::getCurrentOut() { return this->team[currentOut]; }

void Player::setCurrentOut(PlayerMove move) {
  if (move.isSwitch)
    this->currentOut = move.pokemon->getName();
}

PlayerMove Player::move() {
  int moveType = rand() % 2;
  PlayerMove move;
  if (!moveType) {
    std::string oldPokemon = this->currentOut;
    std::string switchPokemon = this->currentOut;
    while (switchPokemon == this->currentOut) {
      int randomChoice = rand() % this->team.size();
      auto it = this->team.begin();
      for (int i = 0; i < randomChoice; i++) {
        switchPokemon = it->first;
        it++;
      }
    }
    move.isSwitch = true;
    move.pokemon = &this->team[switchPokemon];
    std::cout << "Player switched from " << oldPokemon << " to "
              << switchPokemon << std::endl;
  } else {
    int i = rand() % 4;
    move.isSwitch = false;
    move.pokemon = &this->team[this->currentOut];
    move.moveName = this->team[this->currentOut].getMove(i);
    std::cout << this->currentOut << " used "
              << this->team[this->currentOut].getMove(i) << std::endl;
  }
  return move;
}

Pokemon Player::getPokemon(std::string name) { return this->team[name]; }

bool Player::isWinner() { return this->hasWon; }


void Player::buildTeam(std::string filepath) {
  std::ifstream infile(filepath);
  std::string line;
  std::string pokename;
  bool flagPokename = true; // Is the next line a pokemon name?

  while (std::getline(infile, line)) {
    if (line.empty()) {
      flagPokename = true;
      continue;
    }

    if (flagPokename) {
      pokename = line;
      this->team.insert(
          std::pair<std::string, Pokemon>(pokename, *new Pokemon(pokename)));
      flagPokename = false;
    } else {
      // Line is indicitive of a move
      this->team[pokename].addMove(line);
    }
  }
}

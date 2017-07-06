// Player function definitions
#include "Player.h"

Player::Player(std::string filepath, int id) {
  // srand((unsigned)time(nullptr)); //TODO: there is almost definitely a better
  // place for this
  this->hasWon = false;
  this->buildTeam(filepath);
  this->currentOut = this->team.begin()->first;
  this->id = id;
}

/* Receives two moves that have had their damage calculated and their damage
field filled in. Updates which Pokemon is currently on the field. Updates damage
to current Pokemon as specified by the damage field of opponentMove.

This method returns true if a Player's active Pokemon faints as a result of the
opponent's move.

 */
void Player::processTurn(PlayerMove yourMove, PlayerMove opponentMove) {
  this->setCurrentOut(yourMove);
  if (opponentMove.isSuccess == true) {
    this->getCurrentOut()->takeDamage(opponentMove.damage);
    if (yourMove.pokemon->getHP() <= 0) {
      yourMove.pokemon->setFainted(true);
      std::cout << "Player " << this->id << "'s " << this->currentOut << " fainted!\n";
      PlayerMove dummyOppMove;
      dummyOppMove.isSuccess = false;
      PlayerMove newMove = this->makeSwitchOnFaint();
      this->processTurn(newMove, dummyOppMove);
      // this->team.erase(yourMove.pokemon->getName());
    }
  }
}

Pokemon* Player::getCurrentOut() {
  return &(this->team[currentOut]);
}

void Player::setCurrentOut(PlayerMove move) {
  if (move.isSwitch)
    this->currentOut = move.pokemon->getName();
}

bool compMoves(PlayerMove *p1Move, PlayerMove *p2Move) {
  // switches always go first-- both switching is irrelevant
  if (p1Move->isSwitch) { return true; }
  else if (p2Move->isSwitch) { return false; }

  int p1speed = p1Move->pokemon->getStats()["speed"];
  int p2speed = p2Move->pokemon->getStats()["speed"];

  // handle speed ties TODO: more random RNG
  if (p1speed == p2speed) {
    
    int random = rand() % 2;
    if ((random % 2) == 1) {
      return true;
    } else {
      return false;
    }
  }

  return p1speed > p2speed;
}

// PlayerMove Player::move() {
//   int moveType = rand() % 2;
//   PlayerMove move;
//   if (!moveType) {
//     std::string oldPokemon = this->currentOut;
//     std::string switchPokemon = this->currentOut;
//     while (switchPokemon == this->currentOut) {
//       int randomChoice = rand() % this->team.size();
//       auto it = this->team.begin();
//       for (int i = 0; i < randomChoice; i++) {
//         switchPokemon = it->first;
//         it++;
//       }
//     }
//     move.isSwitch = true;
//     move.pokemon = &this->team[switchPokemon];
//     std::cout << "Player switched from " << oldPokemon << " to "
//               << switchPokemon << std::endl;
//   } else {
//     int i = rand() % 4;
//     move.isSwitch = false;
//     move.pokemon = &this->team[this->currentOut];
//     move.moveName = this->team[this->currentOut].getMove(i);
//     std::cout << this->currentOut << " used "
//               << this->team[this->currentOut].getMove(i) << std::endl;
//   }
//   return move;
// }

PlayerMove Player::move() {
  PlayerMove move;
  int i = rand() % 4;
  move.isSwitch = false;
  move.pokemon = &this->team[this->currentOut];
  move.moveName = this->team[this->currentOut].getMove(i);
  std::cout << "Player " << this->id << "'s " << this->currentOut << " used "
            << this->team[this->currentOut].getMove(i) << std::endl;
  return move;
}

PlayerMove Player::makeSwitchOnFaint() {
  PlayerMove move;
  std::string oldPokemon = this->currentOut;
  std::string switchPokemon = this->currentOut;
  // Iterate randomly through valid Pokemon to switch.
  while (switchPokemon == oldPokemon ||
         this->team[switchPokemon].isFainted()) {
    int randomChoice = rand() % this->team.size();
    auto it = this->team.begin();
    for (int i = 0; i < randomChoice; i++) {
      switchPokemon = it->first;
      it++;
    }
  }
  move.isSwitch = true;
  move.pokemon = &this->team[switchPokemon];
  std::cout << "Player " << this->id << " switched from " << oldPokemon << " to " << switchPokemon
            << std::endl;
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

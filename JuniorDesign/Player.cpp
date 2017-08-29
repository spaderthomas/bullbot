// Player function definitions
#include "Player.h"
#include "Move.h"

Player::Player(std::string filepath, int id, PlayerMove (*moveFunc)(Player*)) {
  this->hasLost = false;
  this->buildTeam(filepath);
  this->currentOut = this->team.begin()->first;
  this->id = id;
  this->numAlive = 6;
  this->moveFunc = moveFunc;
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

      int alive = 0;
      for (auto it = this->team.begin(); it != team.end(); ++it) {
        alive = it->second.isFainted() ? alive : alive + 1;
      }
      this->numAlive = alive;

      if (this->numAlive == 0) {
        this->hasLost = true;
	return;
      }
      PlayerMove dummyOppMove;
      dummyOppMove.isSuccess = false;
      PlayerMove newMove = this->makeSwitchOnFaint();
      this->processTurn(newMove, dummyOppMove);
      // this->team.erase(yourMove.pokemon->getName());
    }
  }
}

/* Returns a pointer to the currently active Pokemon */
Pokemon* Player::getCurrentOut() {
  return &(this->team[currentOut]);
}

/* Handles setting currently active Pokemon by means of looking at the player's move */
void Player::setCurrentOut(PlayerMove move) {
  if (move.isSwitch)
    this->currentOut = move.pokemon->getName();
}

/* Behavior of this comparison is as follows:
   1. If one move is a switch, it will be greater than the other (higher priority)
   2. If both moves are switches, the first move will be greater (arbitrary -- this ordering
      doesn't matter. Two switches will always happen simultaneously.)
   3. If both moves are attacks, the attacker with higher speed will go first
*/
bool compMoves(PlayerMove *p1Move, PlayerMove *p2Move) {
  // switches always go first-- both switching is irrelevant
  if (p1Move->isSwitch) { return true; }
  else if (p2Move->isSwitch) { return false; }

  int p1speed = p1Move->pokemon->getStats()["speed"];
  int p2speed = p2Move->pokemon->getStats()["speed"];

  if (p1speed == p2speed) { // Handle speed ties    
    int random = rand() % 2;
    if ((random % 2) == 1) {
      return true;
    } else {
      return false;
    }
  }

  return p1speed > p2speed;
}

/* Chooses a random move from the player's current Pokemon, constructs a new
   PlayerMove object, sets fields, and returns. */
PlayerMove Player::move() {
  PlayerMove move = this->moveFunc(this);
  return move;
}

/* Returns a PlayerMove representing a switch to a valid Pokemon */
PlayerMove Player::makeSwitchOnFaint() {
  PlayerMove move;
  std::string oldPokemon = this->currentOut;
  std::string switchPokemon = this->currentOut;
  // Iterate randomly through valid Pokemon to switch.
  while (switchPokemon == oldPokemon || this->team[switchPokemon].isFainted()) {
    auto it = std::next(team.begin(), rand() % team.size());
    switchPokemon = it->first;
  }
  move.isSwitch = true;
  move.pokemon = &this->team[switchPokemon];
  std::cout << "Player " << this->id << " switched from " << oldPokemon
            << " to " << switchPokemon << " after a faint!" << std::endl;
  return move;
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

/* Gets a Pokemon object from a player's team given a corresponding string of the
   Pokemon's name*/
Pokemon Player::getPokemon(std::string name) { return this->team[name]; }

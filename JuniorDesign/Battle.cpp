/*This class handles the logic of battling. That is, it calculates what
  each player needs to know happened on a turn, routes this information between
  players, and keeps track if the battle is over.

  Flow of a battle goes as such:
  1. Both players send a player_move to the Battle object
  2. The battle object determines the order of the moves
  3. Moves are executed in order (this means that an attack is not executed if
  the attacker faints)
  4. Battle sends a battle_move to both players
  5. Player objects then handle updating states
*/
#include "Battle.h"
#include "Util.cpp"

struct Battle {
  Player *p1;
  Player *p2;
  unsigned int turn = 0;
  bool isOver = false;

  Battle(Player *initP1 = nullptr, Player *initP2 = nullptr) {
    p1 = initP1;
    p2 = initP2;
    if (!p1) {
      p1 = new Player("sample-team.txt");
    }
    if (!p2) {
      p2 = new Player("sample-team.txt");
    }
  }

  /* Takes two moves with their damage filled in and determines which moves will
     be executed.
     Moves that will not be executed have their isSuccess value set to false */
  void validateMove(PlayerMove *p1Move, PlayerMove *p2Move) {
    std::vector<PlayerMove *> moves;
    moves.push_back(p1Move);
    moves.push_back(p2Move);
    std::sort(moves.begin(), moves.end(), compMoves);

    if ((moves[1]->pokemon->getHP() - moves[0]->damage) <= 0) {
      moves[1]->isSuccess = false;
    }
    // std::cout << "\nDetermining order of moves...\n";
    // for (auto &move : moves) {
    //   std::cout << move->pokemon->getName() + " used " + move->moveName +
    //   "\n";
    // }
    // std::cout << "------- end order computation --------\n\n";

    // for (auto &move : moves) {
    //   std::cout << move->pokemon->getName() + "'s " + move->moveName + " was
    //   a " + BoolToString(move->isSuccess) + "\n";
    // }
  }

  // Loops a battle until someone wins
  void battleLoop() {
    while (!p1->isWinner() && !p2->isWinner() && turn < 20) {
      printf("\n\nTurn %i:\n", turn);
      PlayerMove p1Move = p1->move();
      PlayerMove p2Move = p2->move();
      calcDamage(&p1Move, &p2Move);
      validateMove(&p1Move, &p2Move);
      p1->processTurn(p1Move, p2Move);
      p2->processTurn(p2Move, p1Move);
      turn++;
      printf("Player 1's %s has %i HP remaining.\nPlayer 2's %s has %i HP "
             "remaining.\n",
             p1->getCurrentOut().getName().c_str(), p1->getCurrentOut().getHP(),
             p2->getCurrentOut().getName().c_str(),
             p2->getCurrentOut().getHP());
      // usleep(3000000);
    }
  }
};

int main() {
  Battle battle;
  battle.battleLoop();
}

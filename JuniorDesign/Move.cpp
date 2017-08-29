#include "Move.h"

PlayerMove randomAttack(Player *p) {
  PlayerMove move;
  int i = rand() % 4;
  move.isSwitch = false;
  move.pokemon = &p->team[p->currentOut];
  move.moveName = p->team[p->currentOut].getMove(i);
  std::cout << "Player " << p->id << "'s " << p->currentOut << " used "
            << p->team[p->currentOut].getMove(i) << std::endl;
  return move;
}

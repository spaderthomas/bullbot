#pragma once

#include "Player.h"
#include <stdlib.h>

struct PlayerMove {
  bool isSwitch;
  Pokemon *pokemon = NULL;
  std::string moveName = "";
  int damage = 0;
  bool isSuccess = true;
};

bool compMoves(PlayerMove *p1Move, PlayerMove *p2Move);

PlayerMove randomAttack(Player *p);

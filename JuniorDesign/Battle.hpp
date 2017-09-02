void validateMove(PlayerMove *p1Move, PlayerMove *p2Move);
int battleLoop();

struct Battle {
  Player *p1;
  Player *p2;
  unsigned int turn = 0;
  bool isOver = false;
  Battle(Player* initP1, Player* initP2);
  int battleLoop();
};

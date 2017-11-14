struct Battle {
  Player players[2];
  unsigned int turn = 0;
  bool isOver = false;
  void init();
  int battleLoop();
  void battleLogic();
};

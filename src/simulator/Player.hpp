struct Battle;

struct Player {
  int numAlive;
  bool hasLost;
  Pokemon team[6];
  int currentOut;
  int id;
  struct PlayerMove {
    bool isSwitch;
    int teamPos;
    std::string moveName = "";
  } curMove;

  typedef Player::PlayerMove (*moveFuncType)(Player *, Battle *);
  moveFuncType moveFunc;

  static Player* create(std::string filepath, int initID, moveFuncType initMoveFunc);
  Pokemon *getCurrentOut();
  Pokemon getPokemon(std::string name);
  void setCurrentOut(PlayerMove move);
  Player::PlayerMove move(Battle *); //make virtual function, implement in subclasses
  void makeSwitchOnFaint();
  
  void buildTeam(std::string filepath);
};

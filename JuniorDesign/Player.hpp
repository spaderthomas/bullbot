class Player {
public:
  Player(std::string filepath, int id, PlayerMove (*moveFunc)(Player*));
  int id;
  Pokemon *getCurrentOut();
  Pokemon getPokemon(std::string name);
  void setCurrentOut(PlayerMove move);
  void processTurn(PlayerMove yourMove, PlayerMove opponentMove);
  PlayerMove move();
  PlayerMove makeSwitchOnFaint();
  PlayerMove (*moveFunc)(Player*);
  int numAlive;
  bool hasLost;
  std::unordered_map<std::string, Pokemon> team;
  std::string currentOut;
  void buildTeam(std::string filepath);
};

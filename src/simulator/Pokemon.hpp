struct Pokemon {
  static Pokemon* create(std::string n);
  bool containsMove(std::string moveName);
  void takeDamage(int damage);
  std::string name;
  std::vector<std::string> types;
  std::vector<std::string> moves; //no stds
  std::unordered_map<std::string, int> stats;
  void addMove(std::string move);
  int hp = 0;
  bool isBurned = false;
  bool isParalyzed = false;
  bool isAsleep = false;
  bool isFrozen = false;
  bool fainted = false;
};

Pokemon* Pokemon::create(std::string n) {
  auto* p = new Pokemon;
  p->name = n;

  // reading from json file
  std::ifstream is("pokemon.json");
  jsoncons::json pokemon;
  is >> pokemon;

  // grab hp, types, stats
  p->hp = pokemon[n]["stats"]["hp"].as<int>();

  for (const auto &type : pokemon[n]["type"].array_range()) {
    p->types.push_back(type.as<std::string>());
  }

  for (const auto &stat : pokemon[n]["stats"].object_range()) {
    p->stats[stat.key()] = stat.value().as<int>();
  }

  return p;
}

bool Pokemon::containsMove(std::string moveName) {
  for (int i = 0; i < this->moves.size(); i++) {
    if (moves[i] == moveName)
      return true;
  }
  return false;
}

void Pokemon::takeDamage(int damage) {
  this->hp = this->hp - damage > 0 ? this->hp - damage : 0;
}

void Pokemon::addMove(std::string line) {
  this->moves.push_back(line);
}

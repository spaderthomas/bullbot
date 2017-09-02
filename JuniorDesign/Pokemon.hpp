

class Pokemon {
public:
  Pokemon(std::string n = "");
  bool containsMove(std::string moveName);
  void takeDamage(int damage);
  std::string name;
  std::vector<std::string> types;
  std::vector<std::string> moves;
  std::unordered_map<std::string, int> stats;
  void addMove(std::string move);
  int hp = 0;
  bool isBurned = false;
  bool isParalyzed = false;
  bool isAsleep = false;
  bool isFrozen = false;
  bool fainted = false;
};


Pokemon::Pokemon(std::string n) {
  this->name = n;

  // reading from json file
  std::ifstream is("pokemon.json");
  jsoncons::json pokemon;
  is >> pokemon;

  // grab hp, types, stats
  this->hp = pokemon[name]["stats"]["hp"].as<int>();

  for (const auto &type : pokemon[name]["type"].array_range()) {
    this->types.push_back(type.as<std::string>());
  }

  for (const auto &stat : pokemon[name]["stats"].object_range()) {
    stats[stat.key()] = stat.value().as<int>();
  }
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

struct PlayerMove {
  int id;
  bool isSwitch;
  Pokemon *pokemon = NULL;
  std::string moveName = "";
  int damage = 0;
  bool isSuccess = true;
};

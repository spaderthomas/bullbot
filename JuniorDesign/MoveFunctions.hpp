bool compMoves(PlayerMove *p1Move, PlayerMove *p2Move);

PlayerMove randomAttack(Player *p) {
  PlayerMove move;
  int i = rand() % 4;
  move.isSwitch = false;
  move.pokemon = &p->team[p->currentOut];
  move.moveName = p->team[p->currentOut].moves[i];
  move.id = p->id;
  std::cout << "Player " << p->id << "'s " << p->currentOut << " used "
            << p->team[p->currentOut].moves[i] << std::endl;
  return move;
}

PlayerMove randomSampling(Player p, Battle battle, int numSamples) {
  std::vector<std::string> alive;
  for (auto it = p.team.begin(); it != p.team.end(); it++) {
    if (!it->second.fainted) {
      alive.push_back(it->second.name);
    } 
  }
  
  Battle* battleCopy = (Battle*) malloc(sizeof(battle));

  // sampling
  for (int i = 0; i < numSamples; i++) {
    battle = *battleCopy;
    PlayerMove move;
    int swap = rand() % 2;
    if (swap) {
      int pkmn = rand() % alive.size();
      move.pokemon = &(p.team[alive[pkmn]]);
      move.isSwitch = true;
      move.id = p.id;
    } else {
      int i = rand() % 4;
      move.id = p.id;
      move.isSwitch = false;
      move.pokemon = &(p.team[p.currentOut]);
      move.moveName = p.team[p.currentOut].moves[i];
    }
  }
}

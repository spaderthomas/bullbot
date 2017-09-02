Battle::Battle(Player* initP1 = nullptr, Player* initP2 = nullptr) {
  p1 = initP1;
  p2 = initP2;
  if (!p1) {
    p1 = new Player("sample-team.txt", 1, &randomAttack);
  }
  if (!p2) {
    p2 = new Player("sample-team.txt", 2, &randomAttack);
  }
}

/* Takes two moves with their damage filled in and determines which moves will
   be executed.
   Moves that will not be executed have their isSuccess value set to false */
void validateMove(PlayerMove *p1Move, PlayerMove *p2Move) {
  std::vector<PlayerMove *> moves;
  // Sort the moves. std::sort doesn't work here because random nature of
  // sorting causes undefined behavior
  if (compMoves(p1Move, p2Move)) { // p1Move has higher priority, goes first
    moves.push_back(p1Move);
    moves.push_back(p2Move);
  } else { // p2Move has higher priority, goes first
    moves.push_back(p2Move);
    moves.push_back(p1Move);
  }

  if (moves[0]->pokemon->hp <= 0) {
    moves[0]->isSuccess = false;
  }
  if ((moves[1]->pokemon->hp - moves[0]->damage) <= 0) {
    moves[1]->isSuccess = false;
  }
}

// Loops a battle until someone wins
int Battle::battleLoop() {
  while (!this->p1->hasLost && !this->p2->hasLost) {
    printf("\n\nTurn %i:\n", turn);
    PlayerMove p1Move =
        this->p1->move(); // Prints what player's pokemon used what move
    PlayerMove p2Move = this->p2->move();
    calcDamage(&p1Move, &p2Move); // Prints damage calculation info
    validateMove(&p1Move, &p2Move);
    this->p1->processTurn(p1Move, p2Move); // Prints faint information, switch on
                                     // faint, and Pokemon remaining
    this->p2->processTurn(p1Move, p2Move);
    turn++;
    printf("Player 1's %s has %i HP remaining.\nPlayer 2's %s has %i HP "
           "remaining.\n",
           this->p1->getCurrentOut()->name.c_str(), this->p1->getCurrentOut()->hp,
           this->p2->getCurrentOut()->name.c_str(),
           this->p2->getCurrentOut()->hp);
    printf("Player %i has %i Pokemon remaining!\n", this->p1->id, this->p1->numAlive);
    printf("Player %i has %i Pokemon remaining!\n", this->p2->id, this->p2->numAlive);
    // usleep(3000000);
  }

  printf("Player %i has %s \nPlayer %i has %s \n", this->p1->id,
         this->p1->hasLost ? "lost" : "won", this->p2->id, this->p2->hasLost ? "lost" : "won");
  return this->p1->hasLost ? this->p2->id : this->p1->id;
}

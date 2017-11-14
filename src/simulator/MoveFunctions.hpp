#define BIAS_PARAM sqrt(2.f)
#define NUM_MCTS_ITERATIONS 2000

//change this
#define len(arr, type) sizeof(arr) / sizeof(type)

#include "libs/FoxLib/Types.hpp"

Player::PlayerMove randomAttack(Player *p, Battle *battle) {
  Player::PlayerMove move;
  int i = rand() % 4;
  move.isSwitch = false;
  move.teamPos = p->currentOut;
  move.moveName = p->team[p->currentOut].moves[i];
  /*std::cout << "Player " << p->id << "'s " << p->team[p->currentOut].name << "
     used "
            << p->team[p->currentOut].moves[i] << std::endl;*/
  return move;
}

Player::PlayerMove moveFromInt(Battle *battle, int moveNum, int curPlayer) {
  Player::PlayerMove move;
  if (moveNum < 4) { // attack
    move.teamPos = battle->players[curPlayer].currentOut;
    move.moveName = battle->players[curPlayer].getCurrentOut()->moves[moveNum];
    move.isSwitch = false;
  } else { // switch
    move.isSwitch = true;
    move.teamPos = moveNum % 6;
  }

  return move;
}

Player::PlayerMove randomSampling(Player *p, Battle *battle) {
  int NUM_SAMPLES = 100;
  // first array is number of wins returned from a move
  // second array is number of samples of that move
  int samples[2][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

  Battle *battleCopy = (Battle *)malloc(sizeof(*battle));
  int cur = p->id;

  // sampling
  for (int indxSample = 0; indxSample < NUM_SAMPLES; indxSample++) {
    memcpy(battleCopy, battle, sizeof(*battle));
    battleCopy->players[0].moveFunc = &randomAttack;
    battleCopy->players[1].moveFunc = &randomAttack;

    // choose a random move
    Player::PlayerMove move;
    int moveNum = rand() % 10;
    if (moveNum < 4) { // attack
      move.teamPos = battleCopy->players[cur].currentOut;
      move.moveName = battleCopy->players[cur].getCurrentOut()->moves[moveNum];
      move.isSwitch = false;
    } else { // switch
      if (battleCopy->players[cur].team[moveNum % 6].fainted) {
        continue;
      }
      move.isSwitch = true;
      move.teamPos = moveNum % 6;
    }

    battleCopy->players[cur].curMove = move;
    battleCopy->players[!cur].move(battleCopy);

    // simulate
    battleCopy->battleLogic();
    int winner = battleCopy->battleLoop();
    if (cur == winner) {
      samples[0][moveNum] += 1;
      samples[1][moveNum] += 1;
    } else {
      samples[1][moveNum] += 1;
    }

    printf("The winner was %i. Random sampling was used.", winner);
  }
  free(battleCopy);

  // calculate win ratios for each move and fill in player's move
  Player::PlayerMove finalMove;
  float bestWinRatio = -1;
  int bestMove;
  for (int indxMove = 0; indxMove < 10; indxMove++) {
    float curRatio = (float)samples[0][indxMove] / (float)samples[1][indxMove];
    if (curRatio > bestWinRatio) {
      bestWinRatio = curRatio;
      bestMove = indxMove;
    }
  }

  if (bestMove < 4) {
    finalMove.teamPos = battle->players[cur].currentOut;
    finalMove.moveName = battle->players[cur].getCurrentOut()->moves[bestMove];
    finalMove.isSwitch = false;
  } else {
    finalMove.isSwitch = true;
    finalMove.teamPos = bestMove % 6;
  }

  return finalMove;
}

struct MCTSNode {
  short move = 0;
  int visited = 0;
  short wins = 0;
  MCTSNode *parent = nullptr;
  MCTSNode *children[10];
};

//initchildren better name
void initMCTSNode(MCTSNode *node) {
  // init children
  fox_for(indxChild, 10) {
    MCTSNode *child = new MCTSNode;
    child->move = indxChild; // Nth node represents Nth move
    child->parent = node;
    child->visited = 0;
    node->children[indxChild] = child;
  }
}

Player::PlayerMove MCTS(Player *p, Battle *battle) {
  MCTSNode root;
  initMCTSNode(&root);
  root.parent = nullptr;

  fox_for(i, NUM_MCTS_ITERATIONS) {
    printf("iteration %i\n", i);
    bool traverse = true;
    int indxBest = -1;
    MCTSNode *cur = &root;

    // find best node to expand on
    while (true) {
      cur->visited += 1;
      // check for unvisited children
      fox_for(indxChild, len(cur->children, MCTSNode *)) {
        if (!cur->children[indxChild]->visited) {
          indxBest = indxChild;
          initMCTSNode(cur->children[indxBest]);
          cur = cur->children[indxBest];
          cur->visited += 1;
          traverse = false;
          break;
        }
      }

      // if we find an unvisited child, break and simulate
      if (!traverse) {
        break;
      }

      // if all are visited, calculate UCB
      float bestUCB = -1.0;
      fox_for(indxChild, len(cur->children, MCTSNode *)) {
        MCTSNode child = *cur->children[indxChild];
        float UCB =
            ((float)child.wins / (float)child.visited) +
            BIAS_PARAM * (sqrt(log((float)child.parent->visited) / (float)child.visited));
        if (UCB > bestUCB) {
          indxBest = indxChild;
          bestUCB = UCB;
        }
      }

      // move to best UCB node and repeat
      cur = cur->children[indxBest];
    }

    //-- simulation
	//*battleCopy = *battle;
    Battle *battleCopy = (Battle *)malloc(sizeof(*battle));
    memcpy(battleCopy, battle, sizeof(*battle));
    battleCopy->players[0].moveFunc = &randomAttack;
    battleCopy->players[1].moveFunc = &randomAttack;

    // gen and set moves
    int curPlayer = p->id == battleCopy->players[0].id ? 0 : 1;
    Player::PlayerMove move = moveFromInt(battleCopy, indxBest, curPlayer);
    battleCopy->players[curPlayer].curMove = move;
    battleCopy->players[!curPlayer].curMove =
        battleCopy->players[!curPlayer].move(battleCopy);

    // simulate and collect result
    battleCopy->battleLogic();
    int win = battleCopy->battleLoop() == curPlayer ? 1 : 0;
    while (cur->parent) {
      cur->wins += win;
      cur = cur->parent;
    }
  }

  int indxBest = -1;
  float bestUCB = -1.0;
  fox_for(indxChild, len(root.children, MCTSNode *)) {
	  MCTSNode child = *root.children[indxChild];
	  float UCB =
		  ((float)child.wins / (float)child.visited) +
		  BIAS_PARAM * (sqrt(log((float)child.parent->visited) / (float)child.visited));
	  if (UCB > bestUCB) {
		  indxBest = indxChild;
		  bestUCB = UCB;
	  }
  }

  Player::PlayerMove move = moveFromInt(battle, indxBest, p->id);
  return move;
}

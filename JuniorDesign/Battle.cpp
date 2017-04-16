/*This class handles the logic of battling. That is, it calculates what
  each player needs to know happened on a turn, routes this information between
  players, and keeps track if the battle is over.

  Flow of a battle goes as such:
  1. Both players send a player_move to the Battle object
  2. The battle object determines the order of the moves
  3. Moves are executed in order (this means that an attack
  is not executed if the attacker faints)
  4. Battle sends a battle_move to both players
  5. Player objects then handle updating states*/
#include "Player.h"
#include <cstdlib>
#include <string>
#include "DamageCalculator.h"

struct Battle {
	Player* p1;
	Player* p2;
	unsigned turn = 0;
	bool isOver = false;

	Battle(Player* initP1 = nullptr, Player* initP2 = nullptr) {
		p1 = initP1;
		p2 = initP2;
		if (!p1) {
			p1 = new Player("sample-team.txt");
		}
		if (!p2) {
			p2 = new Player("sample-team.txt");
		}
	}

	//Loops a battle until someone wins
	void battleLoop() {
		while (!p1->isWinner() && !p2->isWinner()) {
			PlayerMove p1Move = p1->move();
			PlayerMove p2Move = p2->move();
                        calcDamage(&p1Move, &p2Move);
			p1->processTurn(p1Move, p2Move);
			p2->processTurn(p2Move, p1Move);
                        printf("\n%d, hp is %i\n", p1Move.pokemon->isFainted(), p1Move.pokemon->getHP());
                        //printf("HP status: %i\n\n", p1->getCurrentOut().getHP());
		}
	}
};
	
int main() {
	Battle battle;
	battle.battleLoop();
}

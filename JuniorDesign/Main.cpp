/*This class handles the logic of battling. That is, it calculates what
  each player needs to know happened on a turn, routes this information between
  players, and keeps track if the battle is over.

  Flow of a battle goes as such:
  1. Both players send a player_move to the Battle object
  2. The battle object determines the order of the moves
  3. Moves are executed in order (this means that an attack is not executed if
  the attacker faints)
  4. Battle sends a battle_move to both players
  5. Player objects then handle updating states
*/
#include <stdlib.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <unistd.h>
#include <math.h>
#include <typeinfo>
#include "libs/jsoncons/json.hpp"
#include "libs/jsoncons_ext/jsonpath/json_query.hpp"

jsoncons::json types;
jsoncons::json moves;

#define DAMAGE_DIVISOR 255
#define DAMAGE(attack, defense, pow, rand, mult) (((0.84 * (attack) * (pow) / (defense)) + 2) * (mult) * (rand) / 255)

#include "Pokemon.hpp"
#include "Move.hpp"
#include "Player.hpp"
#include "Battle.hpp"

#include "PlayerFunctions.hpp"
#include "MoveFunctions.hpp"
#include "DamageCalculator.hpp"
#include "BattleFunctions.hpp"

int main() {
  Battle battle;
  std::srand(time(0));
  int winner = battle.battleLoop();
  printf("Player %i is the winner", winner);
}

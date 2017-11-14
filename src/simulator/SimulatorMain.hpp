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

#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

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
#include <math.h>
#include <typeinfo>
#include "libs/jsoncons/json.hpp"
#include "libs/jsoncons_ext/jsonpath/json_query.hpp"

jsoncons::json types;
jsoncons::json moves;

#define DAMAGE_DIVISOR 255
#define DAMAGE(attack, defense, pow, rand, mult) (((0.84 * (attack) * (pow) / (defense)) + 2) * (mult) * (rand) / 255)
#define len(arr, type) sizeof(arr) / sizeof(type)

#include "libs/FoxLib/FoxLib.hpp"

#include "Pokemon.hpp"
#include "Player.hpp"
#include "Battle.hpp"

#include "PlayerFunctions.hpp"
#include "MoveFunctions.hpp"
#include "BattleFunctions.hpp"

int SimulatorMain() {
  Battle battle;
  battle.init();
  std::srand(time(0));
  int winner = battle.battleLoop();
  printf("Player %i is the winner", winner);
  for (;;);
  return winner;
}

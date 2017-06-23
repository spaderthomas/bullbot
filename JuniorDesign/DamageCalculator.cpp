#pragma once
#include "DamageCalculator.h"
jsoncons::json types;
jsoncons::json moves;

/* Takes in pointers to two player moves. Uses RNG to calculate damage each of
 * these moves causes. Sets each move's damage field equal to amount of damage
 * calculated. The processTurn() method in Player then takes the moves and
 * modifies the Pokemon's hp stat accordingly. */
void calcDamage(PlayerMove *p1Move, PlayerMove *p2Move) {
  // attack stat, base power, defense stat, random number, multiplier
  std::ifstream typestream("type-chart.json");
  typestream >> types;

  std::ifstream movestream("move-data.json");
  movestream >> moves;

  p1Move->damage = calc(p1Move, p2Move);
  p2Move->damage = calc(p2Move, p1Move);
}

/* Calculates damage with one move as the attacker and the other as the
 * defender. */
int calc(PlayerMove *attacker, PlayerMove *defender) {
  if (attacker->isSwitch) {
    return 0;
  }

  // use the appropriate stats depending on whether move is physical or special
  int attack = moves[attacker->moveName]["category"] == "physical"
                   ? attacker->pokemon->getStats()["attack"]
                   : attacker->pokemon->getStats()["special"];
  int defense = moves[attacker->moveName]["category"] == "physical"
                    ? defender->pokemon->getStats()["defense"]
                    : defender->pokemon->getStats()["special"];
  int pow = moves[attacker->moveName]["power"].as<int>();
  int random = (rand() % 38) + 217;
  float mult = getMultiplier(attacker, defender);
  int damage = pow == 0 ? 0 : floor(DAMAGE(attack, defense, pow, random, mult));

  printf("%s used %s against %s. The multiplier was %.3f. The base power was "
         "%i. The damage was %i.\n",
         attacker->pokemon->getName().c_str(), attacker->moveName.c_str(),
         defender->pokemon->getName().c_str(), mult, pow, damage);
  return damage;
}

/* Move effectiveness varies based on type chart and whether the attacking move
 * is of the same type as the Pokemon using it. This method calculates that
 * effectiveness multiplier*/
float getMultiplier(PlayerMove *attacker, PlayerMove *defender) {
  float mult = 1.0;
  std::vector<std::string> defenderTypes = defender->pokemon->getTypes();
  std::string moveType = moves[attacker->moveName]["type"].as<std::string>();

  // check type matchup for move used against defender types
  for (const auto &dtype : defenderTypes) {
    for (auto it = types[moveType].object_range().begin();
         it != types[moveType].object_range().end(); ++it) {
      std::vector<std::string> thing =
          it->value().as<std::vector<std::string>>();
      for (int i = 0; i < thing.size(); i++) {
        if (thing[i] == dtype) {
          std::string effectiveness = it->key();
          if (effectiveness == "immune") {
            mult *= 0;
          } else if (effectiveness == "nve") {
            mult *= .5;
          } else if (effectiveness == "s-e") {
            mult *= 2;
          }
        }
      }
    }
  }

  // check for stab
  for (int i = 0; i < attacker->pokemon->getTypes().size(); i++) {
    if (moveType == attacker->pokemon->getTypes()[i]) {
      mult *= 1.5;
    }
  }

  return mult;
}

#include "Pokemon.h"
#include "Player.h"

#define DAMAGE_DIVISOR 255
#define DAMAGE(attack, defense, pow, rand, mult) (((0.84 * (attack) * (pow) / (defense)) + 2) * (mult) * (rand) / 255)

std::ifstream moveiostream("move-data.json");
jsoncons::json moves;
moveiostream >> moves;

std::ifstream typesiostream("type-chart.json");
jsoncons::json types;
typesiostream >> types;

/* Takes in pointers to two player moves. Uses RNG to calculate damage each of these moves causes. Sets each move's damage field equal to amount of damage calculated.*/
void calcDamage(PlayerMove *p1Move, PlayerMove *p2Move) {
    // attack stat, base power, defense stat, random number, multiplier
    p1Move->damage = calcP1Attacker(p1Move, p2Move);
    p2Move->damage = calcP2Attacker(p2Move, p1Move);
    
}

int calc(PlayerMove *attacker, PlayerMove *defender) {
    // use the appropriate stats depending on whether move is physical or special
    int attack = moves[attacker->moveName]["category"] == "physical" ? attacker->pokemon->getStats()["attack"] : attacker->pokemon->getStats()["special"];
    int defense = moves[attacker->moveName]["category"] == "physical" ? defender->pokemon->getStats()["defense"] : defender->pokemon->getStats()["special"];
    int pow = moves[attacker->moveName]["power"].as<int>();
    int random = ( rand() % 38 ) + 217;
    printf("Attack: %i\nDefense: %i\nPower: %i\n", attack, defense, pow);
}

float getMultiplier(PlayerMove *attacker, PlayerMove *defender) {
    float mult = 1.0;
    std::vector<std::string> defenderTypes = defender->getTypes();
    std::string moveType = moves[attacker->moveName][type];
    for (const auto& type : defenderTypes) {
        std::string effectiveness = types[moveType].get("author", "author unknown").as<std::string>();
        type.as<std::string>()
    }
}


    

#include <sstream>
#include "libs/jsoncons/json.hpp"
#include "libs/jsoncons_ext/jsonpath/json_query.hpp"


#define DAMAGE_DIVISOR 255
#define DAMAGE(attack,defense,pow,rand,mult) (((0.84 * (attack) * (pow) / (defense)) + 2) * (mult) * (rand) / 255)

void calcDamage(PlayerMove *p1Move, PlayerMove *p2Move);
int calc(PlayerMove *attacker, PlayerMove *defender);
float getMultiplier(PlayerMove *attacker, PlayerMove *defender);

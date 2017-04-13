#pragma once

#include <fstream>
#include "libs/jsoncons/json.hpp"
#include "libs/jsoncons_ext/jsonpath/json_query.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <tr1/unordered_map>

class Pokemon {
public:
    Pokemon(std::string n = "");
    std::string getName();
    std::vector<std::string> getTypes();
    std::vector<std::string> getMoves();
    std::string getMove(int i);
    std::tr1::unordered_map<std::string, int> getStats();
    void addMove(std::string move);
    bool isFainted();
    bool containsMove(std::string moveName);
    int getHP();
private:
    std::string name;
    std::vector<std::string> types;
    std::vector<std::string> moves;
    std::tr1::unordered_map<std::string, int> stats;
    int hp = 0;
    bool isBurned = false;
    bool isParalyzed = false;
    bool isAsleep = false;
    bool isFrozen = false;
};

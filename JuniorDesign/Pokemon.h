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

class Pokemon {
public:
    Pokemon(std::string n = "");
    std::string getName();
    std::vector<std::string> getTypes();
    std::vector<std::string> getMoves();
    std::string getMove(int i);
    std::vector<std::string> getStats();
    void addMove(std::string move);
    bool isFainted();
    bool containsMove(std::string moveName);
    int getHP();
private:
    std::string name;
    std::vector<std::string> types;
    std::vector<std::string> moves;
    std::vector<std::string> stats;
    std::string strTypes; // temporary just to be able to convert to vectors
    std::string strMoves;
    std::string strStats;
    int hp = 0;
    bool isBurned = false;
    bool isParalyzed = false;
    bool isAsleep = false;
    bool isFrozen = false;
};

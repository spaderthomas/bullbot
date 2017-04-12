#include "Pokemon.h"


    Pokemon::Pokemon(std::string n) {
        this->name = n;

        //reading from json file
        
        std::ifstream is("pokemon.json");
        jsoncons::json pokemon;
        is >> pokemon;

        //getting hp and converting to int
        this->hp = pokemon[name]["stats"]["hp"].as<int>();
        // std::string strHp = pokemon[name]["stats"]["hp"].as<std::string>();
        // std::stringstream convert(strHp);
        // convert >> hp;

        //getting strings from json file with moves, stats and types
        //and removing brackets from strings

        strMoves = pokemon[name]["moves"].as<std::string>();
        if (strMoves.empty()) {
            strMoves = "[ ]";
        }
        strMoves.erase(0, 1);
        strMoves.erase(strMoves.size() - 1);

        strStats = pokemon[this->name]["stats"].as<std::string>();
        if (strStats.empty()) {
            strStats = "{ }";
        }
        strStats.erase(0, 1);
        strStats.erase(strStats.size() - 1);

        strTypes = pokemon[this->name]["type"].as<std::string>();
        if (strTypes.empty()) {
            strTypes = "[ ]";
        }
        strTypes.erase(0, 1);
        strTypes.erase(strTypes.size() - 1);

        //converting all the strings into vectors

        std::stringstream ss(strMoves);
        while (ss.good()) {
            std::string subStr;
            std::getline(ss, subStr, ',');
            moves.push_back(subStr);
        }

        std::stringstream sss(strStats);
        while (sss.good()) {
            std::string subStr;
            std::getline(sss, subStr, ',');
            stats.push_back(subStr);
        }

        std::stringstream ssss(strTypes);
        while (ssss.good()) {
            std::string subStr;
            std::getline(ssss, subStr, ',');
            types.push_back(subStr);
        }
    }

std::string Pokemon::getName() {
    return this->name;
}

int Pokemon::getHP() {
    return this->hp;
}

//returns the vector with the values for types
//but prints them out first
std::vector<std::string> Pokemon::getTypes() {
    for (int i = 0; i < types.size(); i++) {
        std::cout << types[i];
        if (i < types.size() - 1) {
            std::cout << ',' << '\n';
        }
    }
    std::cout << '\n';
    return this->types;
}

//returns the vector with the values for moves
//but prints them out first
std::vector<std::string> Pokemon::getMoves() {
    for (int i = 0; i < this->moves.size(); i++) {
        std::cout << this->moves[i];
        if (i < this->moves.size() - 1) {
            std::cout << ',' << '\n';
        }
    }
    std::cout << '\n';
    return this->moves;
}
std::string Pokemon::getMove(int i)
{
    return this->moves[i];
}

bool Pokemon::containsMove(std::string moveName)
{
    for (int i = 0; i < this->moves.size(); i++)
        {
            if (moves[i] == moveName)
                return true;
        }
    return false;
}

    //returns the vector with the values for stats
    //but prints them out first
    std::vector<std::string> Pokemon::getStats() {
        for (int i = 0; i < this->stats.size(); i++) {
            std::cout << this->stats[i];
            if (i < this->stats.size() - 1) {
                std::cout << ',' << '\n';
            }
        }
        std::cout << '\n';
        return this->stats;
    }

    //adds move to move vector
void Pokemon::addMove(std::string move) {
    this->moves.push_back(move);;
}

    //returns if pokemon is fainted
bool Pokemon::isFainted() {
    return this->hp <= 0;
}


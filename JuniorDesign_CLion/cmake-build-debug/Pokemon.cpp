#include <fstream>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jsonpath/json_query.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace jsoncons;
using namespace jsoncons::jsonpath;
using namespace std;

class Pokemon {
    string name;
    vector<string> types;
    vector<string> moves;
    vector<string> stats;
    string strTypes; // temporary just to be able to convert to vectors
    string strMoves;
    string strStats;
    int hp = 0;
    bool isBurned = false;
    bool isParalyzed = false;
    bool isAsleep = false;
    bool isFrozen = false;

public:
    Pokemon(string n) {
        name = n;
        this->isBurned = isBurned;
        this->isParalyzed = isParalyzed;
        this->isAsleep = isAsleep;
        this->isFrozen = isFrozen;

        //reading from json file

        ifstream is("pokemon.json");
        json pokemon;
        is >> pokemon;

        //getting hp and converting to int

        string strHp = pokemon[name]["stats"]["hp"].as<string>();
        stringstream convert(strHp);
        convert >> hp;

        //getting strings from json file with moves, stats and types
        //and removing brackets from strings

        strMoves = pokemon[name]["moves"].as<string>();
        if (strMoves.empty()) {
            strMoves = "[ ]";
        }
        strMoves.erase(0, 1);
        strMoves.erase(strMoves.size() - 1);

        strStats = pokemon[name]["stats"].as<string>();
        if (strStats.empty()) {
            strStats = "{ }";
        }
        strStats.erase(0, 1);
        strStats.erase(strStats.size() - 1);

        strTypes = pokemon[name]["type"].as<string>();
        if (strTypes.empty()) {
            strTypes = "[ ]";
        }
        strTypes.erase(0, 1);
        strTypes.erase(strTypes.size() - 1);

        //converting all the strings into vectors

        stringstream ss(strMoves);
        while (ss.good()) {
            string subStr;
            getline(ss, subStr, ',');
            moves.push_back(subStr);
        }

        stringstream sss(strStats);
        while (sss.good()) {
            string subStr;
            getline(sss, subStr, ',');
            stats.push_back(subStr);
        }

        stringstream ssss(strTypes);
        while (ssss.good()) {
            string subStr;
            getline(ssss, subStr, ',');
            types.push_back(subStr);
        }
    }

    //returns name

    string getName() {
        return this->name;
    }

    //returns the vector with the values for types
    //but prints them out first
    vector<string> getTypes() {
        for (int i = 0; i < types.size(); i++) {
            std::cout << types[i];
            if (i < types.size() - 1) {
                cout << ',' << '\n';
            }
        }
        cout << '\n';
        return this->types;
    }

    //returns the vector with the values for moves
    //but prints them out first
    vector<string> getMoves() {
        for (int i = 0; i < moves.size(); i++) {
            std::cout << moves[i];
            if (i < moves.size() - 1) {
                cout << ',' << '\n';
            }
        }
        cout << '\n';
        return this->moves;
    }


    //returns the vector with the values for stats
    //but prints them out first
    vector<string> getStats() {
        for (int i = 0; i < stats.size(); i++) {
            cout << stats[i];
            if (i < stats.size() - 1) {
                cout << ',' << '\n';
            }
        }
        cout << '\n';
        return this->stats;
    }

    //adds move to move vector
    void addMove(std::string move) {
        this->moves.push_back(move);;
    }

    //returns if pokemon is fainted
    bool isFainted() {
        return hp <= 0;
    }
};

// tester method to see if it prints out everything correctly

int main() {

    Pokemon abra = Pokemon("Abra");

    cout << abra.getName() << ":\nHas stats: \n";
    abra.getStats();
    cout << "\n";

    cout << "Has moves: \n";
    abra.getMoves();
    cout << "\n";

    cout << "And types: \n";
    abra.getTypes();

    cout << '\n';

    Pokemon pikachu = Pokemon("Pikachu");

    cout << pikachu.getName() << ":\nHas stats: \n";
    pikachu.getStats();
    cout << "\n";

    cout << "Has moves: \n";
    pikachu.getMoves();
    cout << "\n";

    cout << "And types: \n";
    pikachu.getTypes();

    cout << "\nHAIL SATAN, POKEMON IS EVIL \n666\n";

    return 0;

}
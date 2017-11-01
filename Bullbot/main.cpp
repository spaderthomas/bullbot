#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <random>
#include <ctime>
#include <functional>
#include <algorithm>
#include <memory>
#include <thread>
#include <chrono>
#include <sstream>
#include <mutex>

// Libraries
#include "json.hpp"
using json = nlohmann::json;

// Defines
typedef unsigned int fuint;
#define fox_for(iterName, iterCount) for (fuint iterName = 0; iterName < (iterCount); ++iterName)
#define fox_iter(iterator, iterable) for (auto iterator = iterable.begin(); iterator != iterable.end(); ++iterator) 
#define fox_iter_json(iter, iterable) for (json::iterator iter = iterable.begin(); iter != iterable.end(); ++iter) 

#include "Data.hpp"
#include "PSConnection.hpp"
#include "BasePSUser.hpp"
#include "PSUser.hpp"


int main() {
  globalGameData.initGameData();
  std::vector<PSUser> agents;
  agents.resize(2);
  for (int i = 0; i < agents.size(); ++i) {
    PSUser &agent = agents[i];
    agent.set_turn_callback(
        [](std::vector<float> data, std::size_t action_size) {
          int action_ciel = action_size - 1;
          std::mt19937 rng(std::time(0));
          std::uniform_int_distribution<> rdist(0, action_ciel);
          return rdist(rng); // random choice
        });
    agent.connect("localhost:8000");
    auto name = "Carbon12345" + std::to_string(i);
    agent.login(name);
    agent.send("|/autojoin lobby");
    agent.accept_format("gen1randombattle");
  }

  for (int i = 0; i < agents.size(); ++i) {
    for (int j = 0; j < agents.size(); ++j) {
      if (j == i) {
        continue;
      }
      agents[i].send("|/challenge " + agents[j].get_username() +
                     ", gen1randombattle");
    }
  }
  while (true) {
    std::cin.get();
  }
  return 0;
}

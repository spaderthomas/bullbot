/* I removed the GameState class. Instead, you just pass in your team and your opponent's team (which is really what the state of the game is) into your action callback.

   PSUser collects game state
   Sends both teams to an action callback function (e.g. MCTS, Q-network, Random)
   Action callback does whatever it needs to do -- checks for available moves, runs simulations with the given teams, feeds forward into the net -- and returns an int corresponding to an action

   Right now I'm passing in references to the callback function, because a lot of the time I'm not changing the teams so why copy. But sometimes I need to make a copy of the teams (like if I'm simulating). Current problem is reconciling the fact that sometimes I want a reference and sometimes I don't.

 */

// STL
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <random>
#include <ctime>
#include <functional>
#include <algorithm>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <locale>

// Libraries
#include "json.hpp"
using json = nlohmann::json;

#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/NetException.h>
#include <Poco/JSON/Parser.h>
using namespace Poco::Net;
using namespace Poco::JSON;

// Data structures 
#include "Data.hpp"

#include <dlib/bayes_utils/bayes_utils.h>

// Typedefs and defines
typedef unsigned int fuint;
#define fox_for(iterName, iterCount) for (fuint iterName = 0; iterName < (iterCount); ++iterName)
#define fox_iter(iterator, iterable) for (auto iterator = iterable.begin(); iterator != iterable.end(); ++iterator) 
#define fox_iter_json(iter, iterable) for (json::iterator iter = iterable.begin(); iter != iterable.end(); ++iter)

typedef std::unique_ptr<std::mutex> mutex_ptr;
typedef std::lock_guard<std::mutex> mutex_guard;

typedef std::vector<int> action_arr_t; 
typedef std::vector<float> fvec_t;
typedef std::vector<PokemonData> team_t;

typedef std::function<int(team_t&, team_t&)> action_callback_t;
typedef std::function<void(fvec_t*, fvec_t*)> observation_callback_t;

#define BURNED 1
#define PARALYZED 2
#define ASLEEP 4
#define FROZEN 8

#define DEFAULT_NUM_SIMULATIONS 10
// Source code
#include "EnvironmentSettings.hpp"
#include "PSConnection.hpp"
#include "BasePSUser.hpp"
#include "PSUser.hpp"
#include "ai/MoveUtil.hpp"
#include "ai/Simulator.hpp"
#include "ai/MoveFunctions.hpp"


int main() {
	globalGameData.initGameData();
	std::vector<PSUser> agents;
	agents.resize(1);
	for (int i = 0; i < agents.size(); ++i) {
		PSUser& agent = agents[i];
    agent.connect("sim.smogon.com:8000");
		//agent.connect("localhost:8000");
		//agent.username = "spaderbot" + std::to_string(i);
		agent.username = std::string("spaderbot");
		agent.login(agent.username);
		agent.send("|/autojoin lobby");
		agent.accept_format("gen1randombattle");
	}

	for (int i = 0; i < agents.size(); ++i) {
		for (int j = 0; j < agents.size(); ++j) {
			if (j == i) {
				continue;
			}
			agents[i].send("|/challenge " + agents[j].get_username() + ", gen1randombattle");
		}
	}
  
	while (true) {
		std::cin.get();
	}
	return 0;
}

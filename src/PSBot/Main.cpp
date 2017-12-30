// TODO
  // find out why the network stuff is so fucking flaky
  // fix that first sent out pokemon is unregistered
  // correct level parsing for rand and nonrand simultaneously (switch and request)


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
using namespace std;

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
#include <Poco/Timespan.h>
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::JSON;

#include <dlib/bayes_utils/bayes_utils.h>

// Typedefs and defines
typedef unsigned int fuint;
#define fox_for(iterName, iterCount) for (fuint iterName = 0; iterName < (iterCount); ++iterName)
#define fox_iter(iterator, iterable) for (auto iterator = iterable.begin(); iterator != iterable.end(); ++iterator) 
#define fox_iter_json(iter, iterable) for (json::iterator iter = iterable.begin(); iter != iterable.end(); ++iter)

typedef unique_ptr<mutex> mutex_ptr;
typedef lock_guard<mutex> mutex_guard;

typedef vector<int> action_arr_t; 
typedef vector<float> fvec_t;

// Data structures 
#include "Data.hpp"

typedef function<int(team_t&, team_t&, action_arr_t&)> action_callback_t;
typedef function<void(fvec_t*, fvec_t*)> observation_callback_t;

#define BURNED 1
#define PARALYZED 2
#define ASLEEP 4
#define FROZEN 8

#define DAMAGE(attack, defense, pow, rand, mult) (((0.84 * (attack) * (pow) / (defense)) + 2) * (mult) * (rand) / 255)

#define isSwitch(move) move > 3
#define toSwitch(move)
#define switchToIndx(move) (move) - 4

// Algorithm parameters
#define DEFAULT_NUM_SIMULATIONS 10

enum USER_ID {
  PLAYER = 0,
  OPPONENT = 1
};

// Source code
#include "PSConnection.hpp"
#include "BasePSUser.hpp"
#include "ai/Simulator.hpp"
#include "ai/MoveFunctions.hpp"
#include "PSUser.hpp"


// Main loop
int main() {
 	globalGameData.initGameData();
  PSUser agent;
  agent.acceptedFormats.insert("gen1randombattle");
  agent.acceptedFormats.insert("gen1ou");
	agent.username = string("spaderbot");
  //agent.connect("sim.smogon.com:8000");
	agent.connect("localhost:8000");
  agent.login(agent.username);
  agent.send ("|/autojoin lobby");

	while (true) {
		cin.get();
	}
  
	return 0;
}

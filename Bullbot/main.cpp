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

// Typedefs and defines
typedef unsigned int fuint;
#define fox_for(iterName, iterCount) for (fuint iterName = 0; iterName < (iterCount); ++iterName)
#define fox_iter(iterator, iterable) for (auto iterator = iterable.begin(); iterator != iterable.end(); ++iterator) 
#define fox_iter_json(iter, iterable) for (json::iterator iter = iterable.begin(); iter != iterable.end(); ++iter)

typedef std::unique_ptr<std::mutex> mutex_ptr;
typedef std::lock_guard<std::mutex> mutex_guard;

typedef std::vector<int> action_arr_t; 
typedef std::vector<float> fvec_t;


// typedef int (*action_callback_t)(fvec_t*, action_arr_t*);
// typedef int (*observation_callback_t)(fvec_t*, fvec_t*);

typedef std::function<int(fvec_t*, action_arr_t*)> action_callback_t;
typedef std::function<void(fvec_t*, fvec_t*)> observation_callback_t;

// Source code
#include "EnvironmentSettings.hpp"
#include "Data.hpp"
#include "PSConnection.hpp"
#include "BasePSUser.hpp"
#include "PSUser.hpp"

int main() {
	globalGameData.initGameData();
	std::vector<PSUser> agents;
	agents.resize(2);
	for (int i = 0; i < agents.size(); ++i) {
		PSUser& agent = agents[i];
		agent.connect("localhost:8000");
		agent.username = "Carbon12345" + std::to_string(i);
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

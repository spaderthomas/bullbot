// TODO
// find out why the network stuff is so fucking flaky
// correct level parsing for rand and nonrand simultaneously (switch and request)

// STL
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <random>
#include <ctime>
#include <chrono>
#include <functional>
#include <algorithm>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <locale>
#include <stdexcept>
#include <memory>
#include <ctgmath>
using namespace std;

// Libraries
#include "json/json.hpp"
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

#include "Nemesis/Estimator.hpp"
#include "Nemesis/Math.hpp"
#include "Nemesis/TDLearner.hpp"
#include "Nemesis/NeuralNetwork/Activation.hpp"
#include "Nemesis/NeuralNetwork/NeuralNetwork.hpp"
#include "Nemesis/NeuralNetwork/MultiLayerPerceptron.hpp"



// Typedefs and defines
typedef unsigned int uint;
#define fox_for(iterName, iterCount) for (uint iterName = 0; iterName < (iterCount); ++iterName)
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



// Util
enum USER_ID {
	PLAYER = 0,
	OPPONENT = 1
};
std::mt19937 rng(std::time(0));
TDLearner<MLP<float, float, float>> learner;



// Source code
#include "PSConnection.hpp"
#include "BasePSUser.hpp"
#include "ai/Simulator.hpp"
#include "ai/MoveFunctions.hpp"
#include "PSUser.hpp"
#include "ai/TeamPrediction.hpp"




// Main loop
int main() {
	//build_test_net();
	// Let's learn 2x + 3y!

	// First, a network with 2 inputs and 1 output
	MLP<float, float, float> test_mlp(2, 1);

	LinearActivation<float> activator;
	Layer<float> middle_layer(4, activator);
	Layer<float> output_layer(1, activator);

	test_mlp.append_layer(middle_layer);
	test_mlp.append_layer(output_layer);

	test_mlp.finalize();



	globalGameData.initGameData();
	PSUser agent;
	agent.acceptedFormats.insert("gen1randombattle");
	agent.acceptedFormats.insert("gen1ou");
	agent.username = string("spaderbot");
	//agent.connect("sim.smogon.com:8000");
	agent.connect("localhost:8000");
	agent.login(agent.username);
	agent.send("|/autojoin lobby");

	while (true) {
		cin.get();
	}

	return 0;
}

#include <string>
#include <iostream>
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
#include "PSConnection.hpp"
#include "PSUser.hpp"

int main() {

	std::vector<PSUser> agents;
	agents.resize(3);
	for (int i = 0; i < agents.size(); ++i) {
		PSUser& agent = agents[i];
		agent.set_turn_callback([](std::vector<float> data, std::size_t action_size) {
			int action_ciel = action_size - 1;
			std::mt19937 rng(std::time(0));
			std::uniform_int_distribution<> rdist(0, action_ciel);
			return rdist(rng);// random choice
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
			agents[i].send("|/challenge " + agents[j].get_username() + ", gen1randombattle");
		}
	}
	while (true) {
		std::cin.get();
	}
	std::cin.get();
	return 0;
}
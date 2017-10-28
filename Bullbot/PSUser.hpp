#pragma once
#include <string>
#include <iostream>
#include <random>
#include <ctime>
#include <functional>
#include <algorithm>
#include <memory>
#include <thread>
#include <chrono>
#include <sstream>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <experimental/coroutine>
#include <Poco/Net/NetException.h>
#include <Poco/JSON/Parser.h>
#include "BasePSUser.hpp"
#include "EnvironmentData.hpp"

std::string OrderedCallbackCode(std::string raw_state) {
	Parser parser;
	Object::Ptr data = parser.parse(raw_state).extract<Object::Ptr>();
	auto force_switch = data->has("forceSwitch") ? true: false; //todo-expand this
	auto side = data->get("side").extract<Object::Ptr>();
	auto pokedata = side->get("pokemon").extract<Array::Ptr>();

	std::vector<PokemonData> poke_team;
	for (int i = 0; i < pokedata->size(); ++i) {
		PokemonData p;
		auto obj = pokedata->getObject(i);
		p.active = obj->getValue<bool>("active");
		if (p.active && data->has("active")) {
			auto active_pmons = data->get("active").extract<Array::Ptr>();
			auto active_first = active_pmons->getObject(0);
			if (active_first->has("trapped")) {
				p.is_trapped = active_first->getValue<bool>("trapped");
			}
			if (active_first->has("moves")) {
				auto active_moves = active_first->getArray("moves");
				for (int j = 0; j < active_moves->size(); ++j) {
					auto active_move = active_moves->getObject(j);
					MoveData m;
					m.name = active_move->getValue<std::string>("id");
					if (active_move->has("pp")) {
						m.pp = active_move->getValue<short>("pp");
					}
					p.move_data.push_back(m);
				}
			}
		} else {
			auto moves = obj->getArray("moves");
			p.move_data.clear();
			for (int j = 0; j < moves->size(); ++j) {
				MoveData m;
				auto movename = moves->getElement<std::string>(j);
				m.name = movename;
				m.pp = 16; //assume 16
				p.move_data.push_back(m);
			}
		}

		auto stats = obj->getObject("stats");
		p.base_ability = obj->getValue<std::string>("baseAbility");
		auto cond_string = obj->getValue<std::string>("condition");
		auto cond_stream = std::stringstream(cond_string);
		std::string cond_token;

		//hp
		std::getline(cond_stream, cond_token, ' ');
		int cpos = std::string::npos;
		if ((cpos = cond_token.find('/')) == std::string::npos) {
			p.curr_condition = 0;
			p.best_condition = 0;
		} else {
			p.curr_condition = std::stoi(cond_token.substr(0, cpos));
			p.best_condition = std::stoi(cond_token.substr(cpos + 1));
		}

		//status
		std::getline(cond_stream, cond_token, ' ');
		if (cond_token == "fnt") {
			p.is_fainted = true;
		}

		auto lvl_string = obj->getValue<std::string>("details");
		p.lvl = std::stoi(lvl_string.substr(lvl_string.find("L") + 1));
		p.ident = obj->getValue<std::string>("ident");
		p.item = obj->getValue<std::string>("item");
		p.pokeball = obj->getValue<std::string>("pokeball");
		p.attack = stats->getValue<short>("atk");
		p.defense = stats->getValue<short>("def");
		p.special_attack = stats->getValue<short>("spa");
		p.special_defense = stats->getValue<short>("spd");
		p.speed = stats->getValue<short>("spe");

		poke_team.push_back(p);
	}

	// originally from turn command
	// send battle data to agent
	// get battle action values
	// choose action
	auto& pmon = poke_team;
	std::vector<std::string> available_moves;
	std::vector<std::string> available_switches;
	int mv_size = 0;
	int m_index = 1;
	int p_index = 1;
	bool can_switch = true;

	for (auto each : pmon) {
		if (each.active && each.is_trapped) {
			can_switch = false;
			break;
		}
	}

	for (auto each : pmon) {
		if (!each.is_fainted) {
			if (each.active) {
				mv_size = each.move_data.size();
				for (auto mv : each.move_data) {
					if (!force_switch) {
						available_moves.push_back(std::to_string(m_index));
					}
					m_index++;
				}
			} else if (can_switch) {
				available_switches.push_back(std::to_string(p_index));
			}
		}
		p_index++;
	}

	std::mt19937 rng(std::time(0));
	// moves 0-5 + switches 0-4 
	int action_ciel = available_switches.size() + available_moves.size() - 1;
	std::uniform_int_distribution<int> dist(0, action_ciel);
	auto aa = dist(rng);
	if (aa < available_moves.size()) {
		return "move " + available_moves[aa];
	} else if (available_switches.size() > 0) {
		aa -= available_moves.size();
		return "switch " + available_switches[aa];
	}
	return "pass";
}



struct RawState {
	std::string environment_data;
	std::string action_data;
	std::string result;
};

struct PSUser : BasePSUser {
	PSUser() {
		connection.set_on_message(std::bind(&PSUser::handle_message, this, std::placeholders::_1));
	}
	action_callback_t action_callback = OrderedCallbackCode;
	observation_callback_t observation_callback;

	std::unordered_set<std::string> accepted_formats;

	std::string state_str="";
	std::string last_action_str;

	void set_turn_callback(action_callback_t turn_callback) {
		this->action_callback = turn_callback;
	}
	
	void connect(std::string uri) {
		std::lock_guard<std::mutex> lock(*data_mutex_ptr.get());
		try {
			connection.connect(uri);
		} catch (NetException &e) {
			std::printf("Error: %s\n", e.what());
		}
	}

	void login(std::string username, std::string password = "") {
		static const std::string server_url = "play.pokemonshowdown.com";
		std::string response;
		// wait for challenge credentials to be received
		while (chall_str == "" || chall_id == "") {}
		// login without a password
		if (password == "") {
			response = request_action(HTTPRequest::HTTP_GET, "getassertion",
									  "&userid=" + username
									  + "&challengekeyid=" + chall_id
									  + "&challenge=" + chall_str);

			connection.send_msg("|/trn " + username + ",0," + response);
		} else {
			response = request_action(HTTPRequest::HTTP_POST, "login",
									  "&name=" + username
									  + "&pass=" + password
									  + "&challengekeyid=" + chall_id
									  + "&challenge=" + chall_str);
			Poco::JSON::Parser parser;
			if (response.length() > 0) {
				auto result = parser.parse(response.substr(1));
				response = result.extract<Object::Ptr>()->getValue<std::string>("assertion");
			}
			connection.send_msg("|/trn " + username + ",0," + response);
		}
	}

	void challenge(std::string username, std::string battle_type) {
		send("|/challenge " + username + "," + battle_type);
	}

	void accept_challenge(std::string username) {
		send("|/accept " + username);
	}

	void accept_format(std::string format) {
		std::lock_guard<std::mutex> lock(*data_mutex_ptr.get());
		accepted_formats.insert(format);
	}

	void handle_message(std::string message) {
		static const std::string battle_prefix = "battle";
		if (message.length() == 0) return;
		std::string room = "";

		std::istringstream ss(message);
		std::string line;
		int i = 0;
		std::vector<std::vector<std::string>> cmd_line;
		while (std::getline(ss, line)) {
			if (line.length() > 0) {
				std::vector<std::string> cmd_pos;
				std::istringstream line_string(line);
				std::string token;
				while (std::getline(line_string, token, '|')) {
					if (token.length() > 0) {
						cmd_pos.push_back(token);
					}
				}
				if (cmd_pos.size() > 0) {
					cmd_line.push_back(cmd_pos);
				}
			}
		}
		mutex_guard lock(*data_mutex_ptr.get());

		if (cmd_line[0][0].substr(0,1) == ">") {// check if the first string is a room name
			room = cmd_line[0][0].substr(1);// grabs the room name
			if (cmd_line[1].size() > 1) {//ensure the command has an argument
				if (cmd_line[1][0] == "request") {
					std::cout << cmd_line[1][1] << std::endl;
					Parser parser;
					Object::Ptr data = parser.parse(cmd_line[1][1]).extract<Object::Ptr>();
					auto wait = data->has("wait") ? true : false;
					if (!wait) {
						connection.send_msg(room + "|/choose " + action_callback(cmd_line[1][1]) + "|" + "");
					}
					std::cout << std::endl;
				} else if (cmd_line[1][0] == "error") {
					for (int i = 0; i < cmd_line.size(); ++i) {
						for (int j = 0; j < cmd_line[i].size(); ++j) {
							std::cout << " " << cmd_line[i][j] << std::endl;
						}
					}
				} else {
					for (auto each : cmd_line) {
						if (each[0] == "choice") {
							std::cout << "choice" << std::endl;
						} else if (each[0] == "") {
						
						} else if (each[0] == "") {
						}
					}
				}
			} 
		} else {
			std::cout << cmd_line[0][0];

			auto& cmd = cmd_line[0][0];

			if (cmd_line[0].size() > 1) {
				std::cout << "r " << cmd_line[0][1] << std::endl;
			}

			if (cmd == "updateuser") {
				username = cmd_line[0][1];
				is_guest = !(bool(std::stoi(cmd_line[0][2])));
				avatar = cmd_line[0][3];
			} else if (cmd == "challstr") {
				chall_id = cmd_line[0][1];
				chall_str = cmd_line[0][2];
			} else if (cmd == "updatechallenges") {
				Parser parser;
				Object::Ptr challenges = parser.parse(cmd_line[0][1]).extract<Object::Ptr>();
				auto challengesFrom = challenges->get("challengesFrom").extract<Object::Ptr>();
				for (auto piter = challengesFrom->begin(); piter != challengesFrom->end(); ++piter) {
					auto battletype = piter->second.convert<std::string>();
					if (accepted_formats.count(battletype)) {
						std::printf("accepted %s from %s\n", battletype.c_str(), piter->first.c_str());
						connection.send_msg("|/accept " + piter->first);
					}
				}
			}
		
		}
		std::cout << "--------------------------------------" << std::endl;
		//std::cout << message.c_str() << std::endl;
	}
};
#pragma once
#include <algorithm>
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
#include <locale>

std::string OrderedCallbackCode(fvec_t state, action_arr_t available_actions) {
	for (auto each : state) {
		std::cout << each << ", ";
	}
	std::cout << std::endl;
	std::mt19937 rng((unsigned int)std::time(0));
	std::vector<unsigned int> valid_actions;
	bool valid_exist = false;
	for (auto each : available_actions) {
		std::printf("%s\n", each.c_str());
		valid_actions.push_back((each != "") ? 1 : 0);
		valid_exist |= (each != "") ? 1 : 0;
	}
	if (valid_exist) {
		std::discrete_distribution<int> dist(valid_actions.begin(), valid_actions.end());
		auto aa = dist(rng);
		return ((aa < 4) ? std::string("move ") + std::to_string(aa + 1) : std::string("switch ") + std::to_string(aa-2));
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
	std::unordered_map<std::string, EnvironmentData> battle_data;

	void set_action_callback(action_callback_t turn_callback) {
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
			if (room.find("battle") != std::string::npos) {
				std::cout << room << std::endl;
				if (!battle_data.count(room)) {
					battle_data[room] = EnvironmentData();
				}
			}
			if (cmd_line[1].size() > 1) {//ensure the command has an argument
				if (cmd_line[1][0] == "request") {
					std::cout << cmd_line[1][1] << std::endl;
					Parser parser;
					Object::Ptr data = parser.parse(cmd_line[1][1]).extract<Object::Ptr>();
					auto wait = data->has("wait") ? true : false;
					if (!wait) {
						auto force_switch = data->has("forceSwitch") ? true : false; //todo-expand this
						auto side = data->get("side").extract<Object::Ptr>();
						auto pokedata = side->get("pokemon").extract<Array::Ptr>();
						battle_data[room].player_id = side->get("id").extract<std::string>();

						std::array<PokemonData, 6>& poke_team = battle_data[room].player_team;
						for (int i = 0; i < pokedata->size(); ++i) {
							PokemonData p;
							auto obj = pokedata->getObject(i);
							p.active = obj->getValue<bool>("active");
							if (p.active && data->has("active")) {
								auto active_pmons = data->get("active").extract<Array::Ptr>();
								//the first is normally the active pokemon if there is an active pokemon
								auto active_first = active_pmons->getObject(0);
								if (active_first->has("trapped")) {
									p.is_trapped = active_first->getValue<bool>("trapped");
								} else {
									p.is_trapped = false;
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
										if (active_move->has("disabled")) {
											m.disabled = active_move->getValue<bool>("disabled");
										}
										p.move_data[j] = m;
									}
								}
							} else {
								auto moves = obj->getArray("moves");
								for (int j = 0; j < moves->size(); ++j) {
									MoveData m;
									auto movename = moves->getElement<std::string>(j);
									m.name = movename;
									m.pp = 16; //assume 16
									p.move_data[j] = m;
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
							} else {
								p.curr_condition = std::stoi(cond_token.substr(0, cpos));
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
							auto ident = obj->getValue<std::string>("ident");
							p.name = ident.substr(ident.find(" ") + 1);

							poke_team[i] = p;
						}

						// purpose of this section: creating a validity vector of strings, empty strings should be considered false;
						// the strings are also ordered properly(if you'd rather take that approach, which may be better for this problem)
						auto& pmon = poke_team;
						std::array<std::string, 10> available_moves;
						std::fill(available_moves.begin(), available_moves.end(), "");

						bool can_switch = true;
						for (auto each : pmon) {
							if (each.active && each.is_trapped) {
								can_switch = false;
								break;
							}
						}
						
						action_arr_t available_actions;
						std::fill(available_actions.begin(), available_actions.end(), "");
						for (int i = 0; i < pmon.size(); ++i) {
							auto& each = pmon[i];
							if (!each.is_fainted) {
								if (each.active) {
									for (int mv = 0; mv < each.move_data.size(); ++mv) {
										if (!force_switch && !each.move_data[mv].disabled) {
											available_actions[mv] = each.move_data[mv].name;
										}
									}
								} else if (can_switch) {
									std::printf("test %s\n", each.name.c_str());
									available_actions[i + 3] = each.name;
								}
							}
						}
						auto cmd_msg = room + "|/choose " + action_callback(battle_data[room].as_vector(), available_actions) + "|" + "";
						std::cout << cmd_msg << std::endl;
						connection.send_msg(cmd_msg);
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
						std::vector<float> result_data;
						
						if (each[0] == "choice") {
							std::cout << "choice: " << each[1]<< std::endl;
						} else if (each[0] == "switch") {
							std::cout << "switched: " << each[1] << std::endl;
						} else if (each[0] == "-damage") {
							std::cout << "damage-: " << each[1] << std::endl;
						} else if (each[0] == "-heal") {
							std::cout << "healed-: " << each[1] << std::endl;
						} else if (each[0] == "") {
						} else {
							std::cout << "something " << each[0] << std::endl;
						}
						if (observation_callback) {
							observation_callback(battle_data[room].as_vector(), result_data);
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
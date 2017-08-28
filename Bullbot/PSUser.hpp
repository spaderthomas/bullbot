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

struct PSUser : BasePSUser {
	PSUser() {
		connection.set_on_message(std::bind(&PSUser::handle_message, this, std::placeholders::_1));
	}
	typedef std::function<std::size_t(std::vector<float>, std::size_t)> turn_callback_t;
	turn_callback_t turn_callback;
	
	std::unordered_set<std::string> accepted_formats;

	void set_turn_callback(turn_callback_t turn_callback) {
		this->turn_callback = turn_callback;
	}
	
	void connect(std::string uri) {
		std::lock_guard<std::mutex> lock(*data_mutex.get());
		try {
			connection.connect(uri);
		} catch (NetException &e) {
			std::printf("Error: %s\n", e.what());
		}
	}

	void login(std::string username, std::string password = "") {
		static const std::string server_url = "play.pokemonshowdown.com";
		std::string response;
		while (chall_str == "" || chall_id == "") {}
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

		return;
	}

	void challenge(std::string username, std::string battle_type) {
		send("|/challenge " + username + "," + battle_type);
	}

	void accept_challenge(std::string username) {
		send("|/accept " + username);
	}

	void accept_format(std::string format) {
		std::lock_guard<std::mutex> lock(*data_mutex.get());
		accepted_formats.insert(format);
	}


	void handle_message(std::string message) {
		static const std::string battle_prefix = "battle";
		if (message.length() == 0) return;
		if (message == "Timeout") {
			std::cout << "timed out!" << std::endl;
		}

		std::istringstream ss(message);
		std::string line;
		std::unique_ptr<Room> room;
		std::printf("--begin of cmd--\n");
		std::string room_name = "";
		BattleRoom* battle_room = nullptr;
		while (std::getline(ss, line)) {
			if (line.length() < 1) continue;
			std::istringstream line_string(line);
			std::vector<std::string> cmd_pos;
			std::string token;
			while (std::getline(line_string, token, '|')) {
				if (token.length() > 0) {
					cmd_pos.push_back(token);
				}
			}
			std::lock_guard<std::mutex> lock(*data_mutex.get());
			if (cmd_pos.size()) {
				auto& cmd = cmd_pos[0];
				if (cmd == "updateuser") {
					//std::printf("username set to %s\n", cmd_pos[1].c_str());
					username = cmd_pos[1];
					is_guest = !(bool(std::stoi(cmd_pos[2])));
					avatar = cmd_pos[3];
				} else if (cmd == "challstr") {
					std::cout << "update challstr: " << cmd_pos[2] << std::endl;
					chall_id = cmd_pos[1];
					chall_str = cmd_pos[2];
				} else if (cmd == "formats") {
					std::cout << "formats available" << std::endl;
					for (auto each = cmd_pos.begin() + 1; each != cmd_pos.end(); ++each) {
						std::cout << "[" << each->c_str() << "] ";
					}
					std::cout << std::endl;
				} else if (cmd == "c:") { //need to ensure message is sent entirely
					if (cmd_pos.size() > 3) {
						cmd_pos[2].erase(std::remove(cmd_pos[2].begin(), cmd_pos[2].end(), ' '), cmd_pos[2].end());
						std::printf("[%s] %s: %s\n", cmd_pos[1].c_str(), cmd_pos[2].c_str(), cmd_pos[3].c_str());
					}
				} else if (cmd == "l") {
					if (cmd_pos.size() > 1) {
						cmd_pos[1].erase(std::remove(cmd_pos[1].begin(), cmd_pos[1].end(), ' '), cmd_pos[1].end());
						//std::printf("%s left the room.\n", cmd_pos[1].c_str());
					}
				} else if (cmd == "j") {
					if (cmd_pos.size() > 1) {
						cmd_pos[1].erase(std::remove(cmd_pos[1].begin(), cmd_pos[1].end(), ' '), cmd_pos[1].end());
						//std::printf("%s joined the room.\n", cmd_pos[1].c_str());
					}
				} else if (cmd == "updatechallenges") {
					std::printf("challenges updated : %s\n", cmd_pos[1].c_str());
					Parser parser;
					Object::Ptr challenges = parser.parse(cmd_pos[1]).extract<Object::Ptr>();
					auto challengesFrom = challenges->get("challengesFrom").extract<Object::Ptr>();
					for (auto piter = challengesFrom->begin(); piter != challengesFrom->end(); ++piter) {
						auto battletype = piter->second.convert<std::string>();
						if (accepted_formats.count(battletype)) {
							std::printf("accepted %s from %s\n", battletype.c_str(), piter->first.c_str());
							connection.send_msg("|/accept " + piter->first);
						}
					}
				} else if (cmd == "init") {
					for (auto each : cmd_pos) {
						std::cout << "[" << each.c_str() << "] ";
					}
				} else if (cmd == "title") {
					if (battle_room) {
						battle_room->title = cmd_pos[1];
					}
				} else if (cmd[0] == '>') {
					auto room_type = cmd.substr(1, battle_prefix.length());
					room_name = cmd.substr(1);
					if (room_type == battle_prefix) {
						if (!battlerooms.count(room_type)) {
							battlerooms[room_name] = BattleRoom();
						}
						battle_room = &battlerooms[room_type];
					} else {
						chatrooms[room_name] = ChatRoom();
					}
					for (auto each : cmd_pos) {
						std::cout << "[" << each.c_str() << "] ";
					}
					std::cout << std::endl;
				} 
				//else if (cmd == "turn") {


				//}
				else if (cmd == "request") {
					//there is a "wait:true" line in msg
					Parser parser;
					//std::this_thread::sleep_for(std::chrono::milliseconds(200));
					Object::Ptr data = parser.parse(cmd_pos[1]).extract<Object::Ptr>();
					auto side = data->get("side").extract<Object::Ptr>();
					auto pokedata = side->get("pokemon").extract<Array::Ptr>();
					//side->stringify(std::cout);
					std::cout << cmd_pos[1].c_str() << std::endl;
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
						//auto val = pokeobj->get("")->extract <Value::Ptr> ;

						std::cout << p.ident << std::endl;
						poke_team.push_back(p);
					}
					battle_room->teamdata = poke_team;

					// originally from turn command
					// send battle data to agent
					// get battle action values
					// choose action
					auto& pmon = battle_room->teamdata;
					std::vector<std::string> available_moves;
					std::vector<std::string> available_switches;
					int mv_size = 0;
					int m_index = 1;
					int p_index = 1;
					//choose
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
									if (true) {
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
					int action_ciel = available_switches.size() + available_moves.size();
					//give movedata
					//std::uniform_int_distribution<> rdist(0, action_ciel);
					//auto aa = rdist(rng);// choice
					auto aa = turn_callback(std::vector<float>(), action_ciel);
					//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					//std::this_thread::sleep_for(std::chrono::milliseconds(100));

					if (aa < available_moves.size()) {
						auto ostrng = room_name + "|/choose move " + available_moves[aa] + "|" + battle_room->rq_id;
						std::cout << ostrng << std::endl;
						connection.send_msg(ostrng);
					} else if (available_switches.size() > 0) {
						aa -= available_moves.size();
						auto ostrng = room_name + "|/choose switch " + available_switches[aa] + "|" + battle_room->rq_id;
						std::cout << ostrng << std::endl;
						connection.send_msg(ostrng);
					} else {
						auto ostrng = room_name + "|/choose pass" + "|" + battle_room->rq_id;
						std::cout << ostrng << std::endl;
						connection.send_msg(ostrng);
					}

				} else {
					for (auto each : cmd_pos) {
						std::cout << "[" << each.c_str() << "] ";
					}
					std::cout << std::endl;
				}
			}
		}

		std::printf("--end of cmd--\n");
		//std::cout << message.c_str() << std::endl;
	}
};
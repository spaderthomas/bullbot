#pragma once

int RandomAICallback(fvec_t state, action_arr_t available_actions) {
	for (auto each : state) {
		std::cout << each << ", ";
	}
	std::cout << std::endl;
	std::mt19937 rng((unsigned int)std::time(0));
	std::vector<unsigned int> valid_actions(available_actions.size());
	bool valid_exist = false;
	for (int i = 0; i < available_actions.size(); ++i) {
		auto is_valid = (available_actions[i] != "") ? 1 : 0;
		valid_actions[i] = is_valid;
		valid_exist |= is_valid;
	}
	//for (int i = 0; i<available_actions.size(); ++i) {
	//	std::cout<<"move: " << std::to_string(valid_actions[i]) <<" " << available_actions[i]<<std::endl;
	//}
	if (valid_exist) {
		std::discrete_distribution<unsigned int> dist(valid_actions.begin(), valid_actions.end());
		return dist(rng);
	}
	return -1;
}

struct PSUser : BasePSUser {
	PSUser() {
		connection.set_on_message(std::bind(&PSUser::handle_message, this, std::placeholders::_1));
	}
	action_callback_t action_callback = RandomAICallback;
	observation_callback_t observation_callback;

	std::unordered_set<std::string> accepted_formats;

  // Maps room IDs to 
	std::unordered_map<std::string, EnvironmentData> battleData;
	std::unordered_map<std::string, EnvironmentData> oldbattleData;
	unsigned int last_action;

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

	void handle_message(std::string rawMessage) {
		static const std::string battle_prefix = "battle";
		if (rawMessage.length() == 0) return;
		std::string room = "";

		std::istringstream messageStream(rawMessage);
		std::string line;
    
		int i = 0;
		std::vector<std::vector<std::string>> allCurMessageCommands;
    
		//Look through each line of the message
    while (std::getline(messageStream, line)) {
      if (line.length() < 1) {
        continue;
      }

      //Parse message based on PS delimiter '|'
      std::istringstream lineStream(line);
      std::vector<std::string> parsedMessage;
      std::string token;
      while (std::getline(lineStream, token, '|')) {
        if (token.length() > 0) {
          parsedMessage.push_back(token);
        }
      }

      if (parsedMessage.size() > 0) {
        allCurMessageCommands.push_back(parsedMessage);
      }
    }
      mutex_guard lock(*data_mutex_ptr.get());

      auto parsedMessage = allCurMessageCommands[0]; //kill this
			if (parsedMessage.size() > 0) {
        auto messageType = parsedMessage[0];
        
				if (messageType == "request") {
					std::cout << parsedMessage[1] << std::endl;

          json gameStateAsJSON;
          std::istringstream gameStateStream(parsedMessage[1]);
          gameStateStream >> gameStateAsJSON;

          PokemonData team[6];
          bool wait = false; // figure out what this does?
					if (!wait) {
            auto teamData = gameStateAsJSON["side"]["pokemon"];
						fox_for(indxPkmn, teamData.size()) {
              PokemonData newPokemon;
              auto &newPokemonData = teamData[indxPkmn];
              std::string name = newPokemonData["ident"];
              name = name.substr(4, -1); //Remove "p1: "
              
              newPokemon.id = globalGameData.pokemonData[name]["index"];
              std::vector<std::string> types = globalGameData.pokemonData[name]["type"];
              fox_for(indxType, types.size()) {
                auto type = types[indxType];
                newPokemon.types[indxType] = globalGameData.typeData[type]["index"];
              }
              
              if (types.size() == 1) {
                newPokemon.types[1] = -1;
              }
              
              auto moves = newPokemonData["moves"];
              for (int indxMove = 0; indxMove < moves.size(); ++indxMove) {
                std::string curMove = moves[indxMove];
                newPokemon.moves[indxMove] = globalGameData.moveData[curMove]["index"];
              }
              
              auto stats = newPokemonData["stats"];
              newPokemon.stats[0] = stats["atk"];
              newPokemon.stats[1] = stats["def"];
              newPokemon.stats[2] = stats["spa"];
              newPokemon.stats[3] = stats["spd"];
              newPokemon.stats[4] = stats["spe"];

              std::string conditionString = newPokemonData["condition"];
              auto conditionStream = std::stringStream(conditionString);
              std::string conditionToken;

							// Parse weird condition (aka HP) string into HP
              std::getline(conditionStream, conditionToken, ' ');
              const char* condition = conditionString.c_str();
              int numNumbers = 0;
              int indxChar = 0;
              while(char c = condition[indxChar]) {
                if (c - '0' >= 0 && c - '0' <= 9) {
                  numNumbers++;
                  indxChar++;
                } else {
                  break;
                }
              }
              
              std::string intString(condition, numNumbers);
              newPokemon.hp = stoi(intString);

              // check this to see if you really need to
							std::getline(cond_stream, cond_token, ' ');
              
							// Parse out if fainted
							std::getline(cond_stream, cond_token, ' ');
							if (cond_token == "fnt") {
                newPokemon.fainted = true;
							}

              
              std::string lvl_string = newPokemonData["details"];
							newPokemon.level = std::stoi(lvl_string.substr(lvl_string.find("L") + 1));
              
							newPokemon.name = obj->getValue<std::string>("ident");
							newPokemon.name = newPokemon.name.substr(newPokemon.ident.find(" ") + 1);

							team[indxPokemon] = newPokemon;
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
									available_actions[i + 3] = each.name;
								}
							}
						}
						auto aa = action_callback(oldbattleData[room].as_vector(), available_actions);
						auto action_str = (aa < 0) ? std::string("pass"):
							((aa < 4) ? std::string("move ") + std::to_string(aa + 1)
							 : std::string("switch ") + std::to_string(aa - 2));
						std::string cmd_msg = room + "|/choose " + action_str + "|" + "";
						connection.send_msg(cmd_msg);
					}
				} else if (cmd_line[1][0] == "error") {
					for (int i = 0; i < cmd_line.size(); ++i) {
						for (int j = 0; j < cmd_line[i].size(); ++j) {
							std::cout << " " << cmd_line[i][j] << std::endl;
						}
					}
				} else {
					for (auto each : cmd_line) {
						std::vector<float> result_data(4);
						for (auto& each : result_data) {
							each = 0;
						}
						// player damage, player knocked out, enemy damage, enemy knocked out
						if (each[0] == "choice") {
							std::cout << "choice: " << each[1]<< std::endl;
						} else if (each[0] == "switch") {
							std::cout << "switched: " << each[1] << std::endl;
							// add to opponents known pokemon

							if (each[1].find(curr_battle.player_id) == std::string::npos) {
								auto ident = each[1].substr(each[1].find(" ") + 1);
								if (std::none_of(curr_battle.opponent_team.begin(), curr_battle.opponent_team.end(), [&ident](PokemonData& p) {
									return newPokemon.ident == ident;
								})) {
									for (auto& pmon : curr_battle.opponent_team) {
										if (pmon.ident == "") {
											pmon.ident = ident;
											pmon.name = ident;
											break;
										}
									}
								}
							
							}
						} else if (each[0] == "-damage") {
							std::cout << "damage-: " << each[1] << ": " << each[2] << std::endl;
							if (each[1].find(curr_battle.player_id) == std::string::npos) {
								result_data[2] -= 1; // todo: update damage
							} else {
								result_data[0] -= 1;
							}
							//
						} else if (each[0] == "-heal") {
							std::cout << "healed-: " << each[1] << ": " << each[2] << std::endl;
							if (each[1].find(curr_battle.player_id) == std::string::npos) {
								result_data[2] += 1;
							} else {
								result_data[0] += 1;
							}
						} else if (each[0] == "faint") {
							std::cout << "fainted " << each[1] << std::endl;
						} else if (each[0] == "win") {
							std::cout << "win " << each[1] << std::endl;
						} else if (each[0] == "player") {
							std::cout << "You are player: " << each[1] << std::endl;
						} else {
							std::cout << "something " << each[0] << std::endl;
						}
						if (observation_callback) {
							observation_callback(battleData[room].as_vector(), result_data);
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

int RandomAICallback(fvec_t* state, action_arr_t* available_actions) {
	for (auto each : *state) {
		std::cout << each << ", ";
	}
	std::cout << std::endl;
  
	std::mt19937 rng((unsigned int)std::time(0));
	std::vector<fuint> valid_actions(available_actions->size());
	bool valid_exist = false;
	for (int i = 0; i < available_actions->size(); ++i) {
		auto is_valid = ((*available_actions)[i] != "") ? 1 : 0;
		valid_actions[i] = is_valid;
		valid_exist |= is_valid;
	}
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

  // Maps room IDs to game states
	std::unordered_map<std::string, GameState> battleData;

  // Basic web interaction functions
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
		while (chall_str == "" || chall_id == "") {} // wait for credentials to be received
    
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

  // Main PS! interface function
	void handle_message(std::string rawMessage) {
    if (rawMessage.length() == 0) return;

		static const std::string battle_prefix = "battle";
    std::string curRoom = "";

		std::istringstream messageStream(rawMessage);
		std::string line;

    // Each message will be parsed into a vector of string tokens
		std::vector<std::vector<std::string>> parsedMessages;
    
		// Each line of raw message corresponds to one message
    while (std::getline(messageStream, line)) {
      if (line.length() < 1) {
        continue;
      }

      // Parse message based on PS delimiter '|'
      std::istringstream lineStream(line);
      std::vector<std::string> parsedMessage;
      std::string token;
      while (std::getline(lineStream, token, '|')) {
        if (token.length() > 0) {
          parsedMessage.push_back(token);
        }
      }

      if (parsedMessage.size() > 0) {
        parsedMessages.push_back(parsedMessage);
      }
    }
    mutex_guard lock(*data_mutex_ptr.get());
    
    fox_for(indxMessage, parsedMessages.size()) {
      auto parsedMessage = parsedMessages[indxMessage];
      if (parsedMessage.size() > 0) {
        auto messageType = parsedMessage[0];

        if (messageType == "request" && parsedMessage.size() > 1) {
          std::cout << parsedMessage[1] << std::endl;

          json gameStateAsJSON;
          std::istringstream gameStateStream(parsedMessage[1]);
          gameStateStream >> gameStateAsJSON;

          PokemonData team[6];
          bool wait = false; // figure out what this does?
          action_arr_t availableActions;
          if (!wait) {
            auto teamData = gameStateAsJSON["side"]["pokemon"];
            fox_for(indxPkmn, teamData.size()) {
              PokemonData newPokemon;
              auto &newPokemonData = teamData[indxPkmn];

              // Name and ID
              std::string name = newPokemonData["ident"];
              name = name.substr(4, -1); // Remove "p1: "
              newPokemon.name = name;
              newPokemon.id = globalGameData.pokemonData[name]["index"];

              // Types
              std::vector<std::string> types =
                  globalGameData.pokemonData[name]["type"];
              fox_for(indxType, types.size()) {
                auto type = types[indxType];
                newPokemon.types[indxType] =
                    globalGameData.typeData[type]["index"];
              }
              if (types.size() == 1) {
                newPokemon.types[1] = -1;
              }

              // Moves
              auto moves = newPokemonData["moves"];
			  std::string temp = globalGameData.moveData.dump(2);
              for (int indxMove = 0; indxMove < moves.size(); ++indxMove) {
                MoveData newMove;
                newMove.name = moves[indxMove];
                newMove.id = globalGameData.moveData[newMove.name]["index"];
                newPokemon.moves[indxMove] = newMove;
              }

              // Stats
              auto stats = newPokemonData["stats"];
              newPokemon.stats[0] = stats["atk"];
              newPokemon.stats[1] = stats["def"];
              newPokemon.stats[2] = stats["spa"];
              newPokemon.stats[3] = stats["spd"];
              newPokemon.stats[4] = stats["spe"];
              std::string levelString = newPokemonData["details"];
              newPokemon.level =
                  std::stoi(levelString.substr(levelString.find("L") + 1));

              // Parse weird condition string into HP and various effects
              std::string conditionString = newPokemonData["condition"];
              std::istringstream conditionStream(conditionString);
              std::string conditionToken;

              std::getline(conditionStream, conditionToken, ' ');
              const char *condition = conditionString.c_str();
              int numNumbers = 0;
              int indxChar = 0;
              while (char c = condition[indxChar]) {
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
              std::getline(conditionStream, conditionToken, ' ');

              std::getline(conditionStream, conditionToken, ' ');
              if (conditionToken == "fnt") {
                newPokemon.fainted = true;
              }

              // Add new Pokemon to available actions
              if (newPokemon.active) {
                fox_for(indxMove, 4) {
                  availableActions.push_back(newPokemon.moves[indxMove].name);
                }
              } else {
                availableActions.push_back(newPokemon.name);
              }

              team[indxPkmn] = newPokemon;
            }

            // Choose move
            fvec_t dontForgetToMessWithGameState;
            int actionChoice = action_callback(&dontForgetToMessWithGameState,
                                               &availableActions);
            std::string actionString;
            if (actionChoice >= 0) {
              if (actionChoice < 4) { // attack
                actionString = "move " + std::to_string(actionChoice + 1);
              } else { // switch
                actionString = "switch " + std::to_string(actionChoice - 2);
              }
            } else {
              actionString = "pass";
            }

            std::string actionResponse = curRoom + "|/choose " + actionString + "|" + "";
            connection.send_msg(actionResponse);
          }
        } else if (messageType == "error") {
          for (int i = 0; i < parsedMessages.size(); ++i) {
            for (int j = 0; j < parsedMessages[i].size(); ++j) {
              std::cout << " " << parsedMessages[i][j] << std::endl;
            }
          }
        } else if (messageType == "challstr") { // used as login auth
          chall_str = parsedMessage[2];
          chall_id = parsedMessage[1];
        } else if (messageType == "updateuser") {
          username = parsedMessage[1];
        } else if (messageType == "updatechallenges") { // check for valid challenges
          json challenges;
          std::istringstream challengeStream(parsedMessage[1]);
          challengeStream >> challenges;
          
          auto challengesFrom = challenges["challengesFrom"];
          fox_iter_json(challIter, challengesFrom) {
            std::string challengingUser = challIter.key();
            std::string battleType = challIter.value();
            if (accepted_formats.count(battleType)) {
              printf("> accepted a challenge of format: %s from user %s\n",
                     battleType.c_str(),
                     challengingUser.c_str());
              connection.send_msg("|/accept " + challengingUser);
            }
          }
        } else if (messageType.substr(0, 1) == ">") { // PS sending room name
          curRoom = messageType.substr(1);
        }
      }
    // else {
    // for (auto each : parsedMessages) {
    //   std::vector<float> result_data(4);
    //   for (auto& each : result_data) {
    //     each = 0;
    //   }
    //   // player damage, player knocked out, enemy damage, enemy knocked out
    //   if (each[0] == "choice") {
    //     std::cout << "choice: " << each[1]<< std::endl;
    //   } else if (each[0] == "switch") {
    //     std::cout << "switched: " << each[1] << std::endl;
    // add to opponents known pokemon

    //         if (each[1].find(curr_battle.player_id) == std::string::npos) {
    //           auto ident = each[1].substr(each[1].find(" ") + 1);
    //           if (std::none_of(curr_battle.opponent_team.begin(),
    //           curr_battle.opponent_team.end(), [&ident](PokemonData& p) {
    //						return newPokemon.ident ==
    //ident;
    //					})) {
    //						for (auto& pmon : curr_battle.opponent_team)
    //{
    //							if (pmon.ident == "") {
    //								pmon.ident =
    //ident;
    //								pmon.name =
    //ident;
    //								break;
    //							}
    //						}
    //					}
    //
    //				}
    //			} else if (each[0] == "-damage") {
    //				std::cout << "damage-: " << each[1] << ": " << each[2] <<
    //std::endl;
    //				if (each[1].find(curr_battle.player_id) == std::string::npos)
    //{
    //					result_data[2] -= 1; // todo: update
    //damage
    //				} else {
    //					result_data[0] -= 1;
    //				}
    //				//
    //			} else if (each[0] == "-heal") {
    //				std::cout << "healed-: " << each[1] << ": " << each[2] <<
    //std::endl;
    //				if (each[1].find(curr_battle.player_id) == std::string::npos)
    //{
    //					result_data[2] += 1;
    //				} else {
    //					result_data[0] += 1;
    //				}
    //			} else if (each[0] == "faint") {
    //				std::cout << "fainted " << each[1] << std::endl;
    //			} else if (each[0] == "win") {
    //				std::cout << "win " << each[1] << std::endl;
    //			} else if (each[0] == "player") {
    //				std::cout << "You are player: " << each[1] <<
    //std::endl;
    //			} else {
    //				std::cout << "something " << each[0] <<
    //std::endl;
    //			}
    //			if (observation_callback) {
    //				observation_callback(battleData[room].as_vector(),
    //result_data);
    //			}
    //		}
    //	}
    //} else {
    // std::cout << parsedMessages[0][0];

      //     auto& cmd = parsedMessages[0][0];
      
      //   if (parsedMessages[0].size() > 1) {
      //     std::cout << "r " << parsedMessages[0][1] << std::endl;
      //   }

      //   if (cmd == "updateuser") {
      //     username = parsedMessages[0][1];
      //     is_guest = !(bool(std::stoi(parsedMessages[0][2])));
      //     avatar = parsedMessages[0][3];
      //   } else if (cmd == "challstr") {
      //     chall_id = parsedMessages[0][1];
      //     chall_str = parsedMessages[0][2];
      //   } else if (cmd == "updatechallenges") {
      //     Parser parser;
      //     Object::Ptr challenges = parser.parse(parsedMessages[0][1]).extract<Object::Ptr>();
      //     auto challengesFrom = challenges->get("challengesFrom").extract<Object::Ptr>();
      //     for (auto piter = challengesFrom->begin(); piter != challengesFrom->end(); ++piter) {
      //       auto battletype = piter->second.convert<std::string>();
      //       if (accepted_formats.count(battletype)) {
      //         std::printf("accepted %s from %s\n", battletype.c_str(), piter->first.c_str());
      //         connection.send_msg("|/accept " + piter->first);
      //       }
      //     }
      //   }
      //   }
      //   std::cout << "--------------------------------------" << std::endl;
      // }
    }
  }
};

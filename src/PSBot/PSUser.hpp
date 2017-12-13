struct PSUser : BasePSUser {
  // Member variables
  action_callback_t action_callback;
  observation_callback_t observation_callback;
	unordered_set<string> acceptedFormats;

  // Maps room IDs to info for room
  struct BattleData {
    team_t playerTeam;
    team_t oppTeam;
    string playerID;
  };
	unordered_map<string, BattleData> battleData;

  // Constructors
	PSUser() {
		connection.set_on_message(bind(&PSUser::handle_message, this, placeholders::_1));
		action_callback = randomChoice;
	}
  
  // Basic web interaction functions
	void connect(string uri) {
		lock_guard<mutex> lock(*data_mutex_ptr.get());
		try {
			connection.connect(uri);
		} catch (NetException &e) {
			printf("Error: %s\n", e.what());
		}
	}

	void login(string username, string password = "") {
		static const string server_url = "play.pokemonshowdown.com";
		string response;
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
				response = result.extract<Object::Ptr>()->getValue<string>("assertion");
			}
			connection.send_msg("|/trn " + username + ",0," + response);
		}
	}

	void challenge(string username, string battle_type) {
		send("|/challenge " + username + "," + battle_type);
	}

	void accept_challenge(string username) {
		send("|/accept " + username);
	}
 
	void accept_format(string format) {
		lock_guard<mutex> lock(*data_mutex_ptr.get());
		acceptedFormats.insert(format);
  }

	// Parse weird condition string into HP and various effects
  int hpFromPSString(string PSString) {
    const char *condition = PSString.c_str();
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

    string intString(condition, numNumbers);
    return stoi(intString);
  }

  // Main PS! interface function
	void handle_message(string rawMessage) {
    mutex_guard lock(*data_mutex_ptr.get());
    if (rawMessage.length() == 0) return;

		static const string battle_prefix = "battle";
    string curRoom = "";

		istringstream messageStream(rawMessage);
		string line;

    // Each message will be parsed into a vector of string tokens
		vector<vector<string>> parsedMessages;
    
		// Each line of raw message corresponds to one message
    while (getline(messageStream, line)) {
      if (line.length() < 1) {
        continue;
      }

      // Parse message based on PS delimiter '|'
      istringstream lineStream(line);
      vector<string> parsedMessage;
      string token;
      while (getline(lineStream, token, '|')) {
        if (token.length() > 0) {
          parsedMessage.push_back(token);
        }
      }

      if (parsedMessage.size() > 0) {
        parsedMessages.push_back(parsedMessage);
      }
    }
    
    // Loop through each individual message
    fox_for(indxMessage, parsedMessages.size()) {
      auto parsedMessage = parsedMessages[indxMessage];
      if (parsedMessage.size() > 0) {
        auto messageType = parsedMessage[0];

        if (messageType == "request" && parsedMessage.size() > 1) {
          cout << "Received a request for an action\n";
          cout << "|--------------------------------|\n";
          cout << parsedMessage[1] << "\n";
          cout << "|--------------------------------|\n";

          json gameStateAsJSON;
          istringstream gameStateStream(parsedMessage[1]);
          gameStateStream >> gameStateAsJSON;

          // Handle PS edge cases (Hyper Beam trap, swap after faint)
          bool waitForServer = false;
          bool forcedToSwitch = false;
          bool trapped = false;
          if (gameStateAsJSON.find("wait") != gameStateAsJSON.end()) {
            waitForServer = true;
          }
          if (gameStateAsJSON.find("forceSwitch") != gameStateAsJSON.end()) {
            forcedToSwitch = true;
          }
          if (gameStateAsJSON.find("active") != gameStateAsJSON.end()) {
            auto activeData =
                gameStateAsJSON["active"].at(0); // PS stores this as a list?
            if (activeData.find("trapped") != activeData.end()) {
              trapped = true;
            }
          }

          // Construct full player team and valid moves
          team_t team;
          action_arr_t availableActions; 
          if (!waitForServer) {
            auto teamData = gameStateAsJSON["side"]["pokemon"];

            // Mark current pokemon's moves as legal if we aren't forced to switch
            if (!forcedToSwitch) {
              auto validMoves = gameStateAsJSON["active"][0]["moves"];
              fox_for(indxMove, validMoves.size()) {
                availableActions.push_back(indxMove);
              }
            }

            // Construct each individual Pokemon of player team
            fox_for(indxPkmn, teamData.size()) {
              PokemonData newPokemon;
              auto &newPokemonData = teamData[indxPkmn];

              // Name and ID
              string name = newPokemonData["ident"];
              name = name.substr(4, -1); // Remove "p1: "
              initPokemonFromName(newPokemon, name);

              // Moves
              auto moves = newPokemonData["moves"];
              for (int indxMove = 0; indxMove < moves.size(); ++indxMove) {
                MoveData newMove;
                newMove.initFromName(moves[indxMove]);
                newPokemon.moves.push_back(newMove);
              }

              // Stats
              auto& stats = newPokemonData["stats"];
              newPokemon.stats.insert({string("atk"), stats["atk"]});
              newPokemon.stats.insert({string("def"), stats["def"]});
              newPokemon.stats.insert({string("spatk"), stats["spa"]});
              newPokemon.stats.insert({string("spdef"), stats["spd"]});
              newPokemon.stats.insert({string("speed"), stats["spe"]});
              string levelString = newPokemonData["details"];
              newPokemon.level = stoi(levelString.substr(levelString.find("L") + 1));

              // Status effects, current HP
              string conditionString = newPokemonData["condition"];
              istringstream conditionStream(conditionString);
              string conditionToken;
              getline(conditionStream, conditionToken, ' ');

              newPokemon.hp = hpFromPSString(conditionToken);

              getline(conditionStream, conditionToken, ' ');
              if (conditionToken == "fnt") {
                newPokemon.fainted = true;
              }

              if (bool active = newPokemonData["active"]) {
                newPokemon.active = true;
                newPokemon.trapped = trapped;
              }

              // If new Pokemon can be switched to, mark it
              if (!newPokemon.active && !newPokemon.fainted && !trapped) {
                availableActions.push_back(indxPkmn + 3); // map indxPkmn=1 to switch=4 (first switch slot)
              }

              team.push_back(newPokemon);
            }

            battleData[curRoom].playerTeam = team;

            // Choose move
            int actionChoice = action_callback(battleData[curRoom].playerTeam,
                                               battleData[curRoom].oppTeam,
                                               availableActions);
            string actionString;
            if (actionChoice >= 0) {
              if (actionChoice < 4) { // attack
                actionString = "move " + to_string(actionChoice + 1); // PS uses [1:4]
              } else { // switch
                actionString = "switch " + to_string(actionChoice - 2);
              }
            } else {
              actionString = "pass";
            }

            string actionResponse = curRoom + "|/choose " + actionString + "|" + "";
            connection.send_msg(actionResponse);
            cout << "Sent action to server. Action was:\n" << actionResponse << "\n\n";
            }
        } else if (messageType == "error") {
					cout << "Received an error from PS!\n";
          for (auto& message : parsedMessages) {
            for (auto& messagePiece : message) {
              cout << messagePiece << " ";
            }
            cout << endl;
          }
        } else if (messageType == "challstr") { // used as login auth
          chall_str = parsedMessage[2];
          chall_id = parsedMessage[1];
        } else if (messageType == "updateuser") {
          username = parsedMessage[1];
        } else if (messageType == "updatechallenges") { // check for valid challenges
          json challenges;
          istringstream challengeStream(parsedMessage[1]);
          challengeStream >> challenges;

          auto challengesFrom = challenges["challengesFrom"];
          fox_iter_json(challIter, challengesFrom) {
            string challengingUser = challIter.key();
            string battleType = challIter.value();
            if (acceptedFormats.count(battleType)) {
              printf("> accepted a challenge of format: %s from user %s\n", battleType.c_str(), challengingUser.c_str());
              connection.send_msg("|/accept " + challengingUser);
            }
          }
        } else if (messageType.substr(0, 1) == ">") { // PS sending room name
          curRoom = messageType.substr(1);
        } else if (messageType == "player") { // fix bug here: when opponent quits, pm.size==2
          if (parsedMessage[2] == username) {
            battleData[curRoom].playerID = parsedMessage[1];
          }
        } else if (messageType == "switch") {
          // example ["switch", "p1a: Muk", ", L74", "82/100 brn"]
          string curPlayer = parsedMessage[1].substr(0, 2);
          string switchTo = parsedMessage[1].substr(5);
          // add some comments




          
          if (!(curPlayer == battleData[curRoom].playerID)) { // check who switched
            bool switchPokemonExists = false;
            if (getPokemon(switchTo, battleData[curRoom].oppTeam).name != "dummy") {
              switchPokemonExists = true;
            }

            if (!switchPokemonExists) {
              PokemonData newPokemon;
              initPokemonFromName(newPokemon, switchTo);
              newPokemon.hp = hpFromPSString(parsedMessage[3]); // Only a % for opponent
              int levelBeginPos = parsedMessage[2].find("L");
              int level = stoi(parsedMessage[2].substr(levelBeginPos + 1, 2));
              newPokemon.active = true;
              battleData[curRoom].oppTeam.push_back(newPokemon);
            }
          }
        } else if (messageType == "move") {
          string curPlayer = parsedMessage[1].substr(0, 2);
          string attackingPokemon = parsedMessage[1].substr(5);
          string moveName = parsedMessage[2];

          // move name should be lowercase no spaces to match json
          string::iterator endPos = remove(moveName.begin(), moveName.end(), ' ');
          moveName.erase(endPos, moveName.end());
          transform(moveName.begin(), moveName.end(), // input beginning and end
                         moveName.begin(), // output beginning
                         ::tolower); // function

          // Check if opponent's pokemon has used this move before. If not, add it
          if (!(curPlayer == battleData[curRoom].playerID)) {
            PokemonData& activePkmn = getPokemon(attackingPokemon, battleData[curRoom].oppTeam);
            if (activePkmn.hasMove(moveName) == -1) {
              MoveData newMove;
              newMove.initFromName(moveName);
              activePkmn.moves.push_back(newMove);
            }
          }
        } else if (messageType == "faint") {
          string curPlayer = parsedMessage[1].substr(0, 2);          
          string attackingPokemon = parsedMessage[1].substr(5);
          if (!(curPlayer == battleData[curRoom].playerID)) {
            PokemonData& faintedPokemon = getPokemon(attackingPokemon, battleData[curRoom].oppTeam);
            faintedPokemon.fainted = true;
          }
        } else if ((messageType == "-damage") || (messageType == "-heal")) {
          string curPlayer = parsedMessage[1].substr(0,2);
          string affectedPokemonName = parsedMessage[1].substr(5);
          if (!(curPlayer == battleData[curRoom].playerID)) {
            int newHP = hpFromPSString(parsedMessage[2]);
            PokemonData& affectedPokemon = getPokemon(affectedPokemonName, battleData[curRoom].oppTeam);
            affectedPokemon.hp = newHP; 
          }
        } else if ((messageType == "-status") || (messageType == "-curestatus")) {
          string curPlayer = parsedMessage[1].substr(0,2);
          string affectedPokemonName = parsedMessage[1].substr(5);
          if (!(curPlayer == battleData[curRoom].playerID)) {
            string status = parsedMessage[2];
            PokemonData& affectedPokemon = getPokemon(affectedPokemonName, battleData[curRoom].oppTeam);
            if (status == "frz") {
              affectedPokemon.frozen = 
                (messageType == "-status") ? FROZEN : 0; 
            } else if (status == "brn") {
              affectedPokemon.burned = 
                (messageType == "-status") ? BURNED : 0;
            } else if (status == "par") {
              affectedPokemon.paralyzed = 
                (messageType == "-status") ? PARALYZED : 0;
            } else if (status == "slp") {
              affectedPokemon.asleep = 
                (messageType == "-status") ? ASLEEP : 0;
            }
         }
        } else if (messageType == "-boost") {
          string curPlayer = parsedMessage[1].substr(0,2);
          string affectedPokemonName = parsedMessage[1].substr(5);
          string affectedStat = parsedMessage[2];
          int boost = stoi(parsedMessage[3]);
          if (!(curPlayer == battleData[curRoom].playerID)) {
            PokemonData& affectedPokemon = getPokemon(affectedPokemonName, battleData[curRoom].oppTeam);
            affectedPokemon.boosts[affectedStat] = boost;
          }
        } else if (messageType == "win") {
          string winnerUsername = parsedMessage[1];
          if (winnerUsername == username) {
            // do something
          }
        } else {
          // this makes a lot of spam for chat joins and whatnot
          //cout << "Unhandled message type. Message type was:\n" << parsedMessage[0] << "\n\n";
        }
      } else {
        cout << "Message was empty!\n\n";
      }
    }
  }
};

// look at updating challenges. make the iteration code work.

#pragma once

#include <string>
#include <iostream>	
#include <fstream>
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

struct PSUser : BasePSUser {
  PSUser() {
    connection.set_on_message(
        std::bind(&PSUser::handle_message, this, std::placeholders::_1));
  }
  typedef std::function<std::size_t(std::vector<float>, std::size_t)>
      turn_callback_t;
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
    while (chall_str == "" || chall_id == "") {
    }
    if (password == "") {
      response = request_action(HTTPRequest::HTTP_GET, "getassertion",
                                "&userid=" + username + "&challengekeyid=" +
                                    chall_id + "&challenge=" + chall_str);

      connection.send_msg("|/trn " + username + ",0," + response);
    } else {
      response = request_action(HTTPRequest::HTTP_POST, "login",
                                "&name=" + username + "&pass=" + password +
                                    "&challengekeyid=" + chall_id +
                                    "&challenge=" + chall_str);
      Poco::JSON::Parser parser;
      if (response.length() > 0) {
        auto result = parser.parse(response.substr(1));
        response =
            result.extract<Object::Ptr>()->getValue<std::string>("assertion");
      }
      connection.send_msg("|/trn " + username + ",0," + response);
    }

    return;
  }

  void challenge(std::string username, std::string battle_type) {
    send("|/challenge " + username + "," + battle_type);
  }

  void accept_challenge(std::string username) { send("|/accept " + username); }

  void accept_format(std::string format) {
    std::lock_guard<std::mutex> lock(*data_mutex.get());
    accepted_formats.insert(format);
  }

  void handle_message(std::string rawMessage) {
    static const std::string battle_prefix = "battle";
    if (rawMessage.length() == 0)
      return;

    std::istringstream messageStream(rawMessage);
    std::string line;

    // PS requires messages to be prefixed with active room
    std::string room = "";

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
      
      std::lock_guard<std::mutex> lock(*data_mutex.get());
      if (parsedMessage.size()) {
        auto &messageType = parsedMessage[0];

        if (messageType == "updateuser") {
          username = parsedMessage[1];
          is_guest = !(bool(std::stoi(parsedMessage[2])));
          avatar = parsedMessage[3];
        } else if (messageType == "challstr") {
          chall_id = parsedMessage[1];
          chall_str = parsedMessage[2];
        } else if (messageType == "c:") {
          // need to ensure message is sent entirely
          if (parsedMessage.size() > 3) {
            parsedMessage[2].erase(
                std::remove(parsedMessage[2].begin(), parsedMessage[2].end(), ' '),
                parsedMessage[2].end());
            std::printf("[%s] %s: %s\n", parsedMessage[1].c_str(), parsedMessage[2].c_str(),
                        parsedMessage[3].c_str());
          }
        } else if (messageType == "l") {
          if (parsedMessage.size() > 1) {
            parsedMessage[1].erase(
                std::remove(parsedMessage[1].begin(), parsedMessage[1].end(), ' '),
                parsedMessage[1].end());
          }
        } else if (messageType == "j") {
          if (parsedMessage.size() > 1) {
            parsedMessage[1].erase(
                std::remove(parsedMessage[1].begin(), parsedMessage[1].end(), ' '),
                parsedMessage[1].end());
          }
        } else if (messageType == "updatechallenges") {
          json challenges;
          std::istringstream challengeMessage(parsedMessage[1]);
		  std::cout << parsedMessage[1];
	          challengeMessage >> challenges;
          
          auto receivedChallenges = challenges["challengesFrom"];

          fox_iter_json(challIter, receivedChallenges) {
            //auto battletype = challIter->second.convert<std::string>();
            //if (accepted_formats.count(battletype)) {
              //std::printf("accepted %s from %s\n", battletype.c_str(), challIter->first.c_str());
              //connection.send_msg("|/accept " + challIter->first);
            //}
          }
        } else if (messageType[0] == '>') { // Message is '>roomid'
          room = messageType.substr(1);
        } else if (messageType == "request") { //Requests a move, sends game state
          if (parsedMessage.size() < 2) { // there is a "wait:true" line in msg
            continue;
          }

          std::cout << "\n---REQUEST BEGIN---\n";
          std::cout << parsedMessage[1].c_str() << std::endl;
          std::cout << "---REQUEST END---\n\n";
          
          json gameStateAsJSON;
          std::istringstream gameStateStream(parsedMessage[1]);
          gameStateStream >> gameStateAsJSON;

          PokemonData team[6];
          auto teamData = gameStateAsJSON["side"]["pokemon"];
          for (int indxPkmn = 0; indxPkmn < teamData.size(); ++indxPkmn) {
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
            
            //Parse weird condition (aka HP) string into HP
            std::string conditionString = newPokemonData["condition"];
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

            newPokemon.active = newPokemonData["active"];

            team[indxPkmn] = newPokemon;
          }

          std::string outstring = room + "|/choose_move " + "0" + "|";
          connection.send_msg(outstring);

      //   if (aa < available_moves.size()) {
      //     auto ostrng = room_name + "|/choose move " + available_moves[aa] +
      //                   "|" + battle_room->rq_id;
      //     // std::cout << ostrng << std::endl;
      //     connection.send_msg(ostrng);
      //   } else if (available_switches.size() > 0) {
      //     aa -= available_moves.size();
      //     auto ostrng = room_name + "|/choose switch " +
      //                   available_switches[aa] + "|" + battle_room->rq_id;
      //     // std::cout << ostrng << std::endl;
      //     connection.send_msg(ostrng);
      //   } else {
      //     auto ostrng = room_name + "|/choose pass" + "|" + battle_room->rq_id;
      //     // std::cout << ostrng << std::endl;
      //     connection.send_msg(ostrng);
      //   }

        // } else {
        //   for (auto each : parsedMessage) {
        //     std::cout << "-[" << each.c_str() << "]- ";
        //   }
        //   std::cout << std::endl;
        // }
        }
      }
    }
  }
};

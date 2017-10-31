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
    
    std::unique_ptr<Room> room;
    std::string room_name = "";
    BattleRoom *battle_room = nullptr;

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
          Parser parser;
          Object::Ptr challenges = parser.parse(parsedMessage[1]).extract<Object::Ptr>();
          auto challengesFrom = challenges->get("challengesFrom").extract<Object::Ptr>();
          
          for (auto piter = challengesFrom->begin(); piter != challengesFrom->end(); ++piter) {
            auto battletype = piter->second.convert<std::string>();
            if (accepted_formats.count(battletype)) {
              std::printf("accepted %s from %s\n", battletype.c_str(), piter->first.c_str());
              connection.send_msg("|/accept " + piter->first);
            }
          }
        } else if (messageType == "init") {
          for (auto each : parsedMessage) {
            std::cout << "[" << each.c_str() << "] ";
          }
        } else if (messageType == "title") {
          if (battle_room) {
            battle_room->title = parsedMessage[1];
          }
        } else if (messageType[0] == '>') {
          auto room_type = messageType.substr(1, battle_prefix.length());
          room_name = messageType.substr(1);
          if (room_type == battle_prefix) {
            if (!battlerooms.count(room_type)) {
              battlerooms[room_name] = BattleRoom();
            }
            battle_room = &battlerooms[room_type];
          } else {
            chatrooms[room_name] = ChatRoom();
          }
          for (auto each : parsedMessage) {
            std::cout << "[" << each.c_str() << "] ";
          }
          std::cout << std::endl;
        } else if (messageType == "request") { //Requests a move, sends game state
          if (parsedMessage.size() < 2) { // there is a "wait:true" line in msg
            continue;
          }
          
          Parser parser;
          json gameStateAsJSON;
          std::istringstream gameStateStream(parsedMessage[1]);
          gameStateStream >> gameStateAsJSON;

          std::cout << "\n---REQUEST BEGIN---\n";
          std::cout << parsedMessage[1].c_str() << std::endl;
          std::cout << "---REQUEST END---\n\n";

          PokemonData team[6];
          auto teamData = gameStateAsJSON["side"]["pokemon"];
          for (int indxPkmn = 0; indxPkmn < teamData.size(); ++indxPkmn) {
            PokemonData newPokemon;
            auto&newPokemonData = teamData[indxPkmn];
            std::string name = newPokemonData["ident"];
            name = name.substr(4, -1); //Remove "p1: "

            //Get types and ID from pokemon.json
            //beep boop code here
            
            auto moves = newPokemonData["moves"];
            for (int indxMove = 0; indxMove < moves.size(); ++indxMove) {
              std::string curMove = moves[indxMove];
              //index into JSON to get index
              newPokemon.moves[indxMove] = indxMove; //placeholder
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
            for (int indxChar = 0; indxChar < sizeof(*condition)/sizeof(condition[0]); indxChar++) {
              char c = condition[indxChar];
              if (c - '0' >= 0 && c - '0' <= 9) {
                numNumbers++;
              } else {
                break;
              }
            }
            std::string intString(condition, numNumbers);
            newPokemon.hp = stoi(intString);

            newPokemon.active = newPokemonData["active"];

            team[indxPkmn] = newPokemon;
        }

        // originally from turn command
        // send battle data to agent
        // get battle action values
        // choose action
        auto &pmon = battle_room->teamdata;
        std::vector<std::string> available_moves;
        std::vector<std::string> available_switches;
        int mv_size = 0;
        int m_index = 1;
        int p_index = 1;
        // choose
        bool can_switch = true;


        std::mt19937 rng(std::time(0));
        int action_ciel = available_switches.size() + available_moves.size();
        // give movedata
        // std::uniform_int_distribution<> rdist(0, action_ciel);
        // auto aa = rdist(rng);// choice
        auto aa = turn_callback(std::vector<float>(), action_ciel);
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (aa < available_moves.size()) {
          auto ostrng = room_name + "|/choose move " + available_moves[aa] +
                        "|" + battle_room->rq_id;
          // std::cout << ostrng << std::endl;
          connection.send_msg(ostrng);
        } else if (available_switches.size() > 0) {
          aa -= available_moves.size();
          auto ostrng = room_name + "|/choose switch " +
                        available_switches[aa] + "|" + battle_room->rq_id;
          // std::cout << ostrng << std::endl;
          connection.send_msg(ostrng);
        } else {
          auto ostrng = room_name + "|/choose pass" + "|" + battle_room->rq_id;
          // std::cout << ostrng << std::endl;
          connection.send_msg(ostrng);
        }

      } else {
        for (auto each : parsedMessage) {
          std::cout << "-[" << each.c_str() << "]- ";
        }
        std::cout << std::endl;
      }
    }
  }
  // std::cout << message.c_str() << std::endl;
}
}
;

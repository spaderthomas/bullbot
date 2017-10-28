#pragma once
#include <string>
#include <iostream>
#include <random>
#include <ctime>
#include <functional>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <experimental/coroutine>
#include <Poco/Net/NetException.h>
#include <Poco/JSON/Parser.h>
#include "Definitions.hpp"
#include "PSConnection.hpp"
#include "EnvironmentData.hpp"

using namespace Poco::Net;
using namespace Poco::JSON;
struct BasePSUser {
	//creates a new base user with its own mutex
	BasePSUser() {
		data_mutex_ptr.reset(new std::mutex());
	}

	std::string request_action(std::string request_type, std::string action, std::string additional_args) {
		static const std::string server_url = "play.pokemonshowdown.com";
		static const std::string path = "/action.php";
		HTTPClientSession session(server_url);
		std::string params = "act=" + action +
			additional_args;
		if (request_type == HTTPRequest::HTTP_POST) {
			HTTPRequest request(request_type, path, HTTPRequest::HTTP_1_1);
			request.setContentType("application/x-www-form-urlencoded");
			request.setContentLength(params.length());
			session.sendRequest(request) << params;
		} else {
			HTTPRequest request(request_type, path + "?" + params, HTTPRequest::HTTP_1_1);
			session.sendRequest(request);
		}
		//send request
		//stream in response
		HTTPResponse resp;
		std::string response;
		try {
			std::istream& is = session.receiveResponse(resp);
			//copy response to a string
			Poco::StreamCopier::copyToString(is, response);
		} catch (Poco::Net::NetException &e) {
			std::cout << e.what() << std::endl;
		}
		return response;
	};

	void send(std::string message) {
		mutex_guard lock(*data_mutex_ptr.get());
		connection.send_msg(message);
	}

	std::string get_username() {
		mutex_guard lock(*data_mutex_ptr.get());
		return username;
	}

	std::string get_avatar() {
		mutex_guard lock(*data_mutex_ptr.get());
		return avatar;
	}

	std::string get_chall_str() {
		mutex_guard lock(*data_mutex_ptr.get());
		return chall_str;
	}

	std::string get_chall_id() {
		mutex_guard lock(*data_mutex_ptr.get());
		return chall_id;
	}

protected:
	PSConnection connection;
	mutex_ptr data_mutex_ptr;

	std::string uri = "localhost:8000";
	std::string username;
	std::string avatar;
	std::string chall_str;
	std::string chall_id;
	std::string assertion;

	bool autojoin = false;
	bool is_guest = true;
};

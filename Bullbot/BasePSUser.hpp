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
#include "PSConnection.hpp"
#include "Data.hpp"

using namespace Poco::Net;
using namespace Poco::JSON;
struct BasePSUser {
	BasePSUser() {
		data_mutex.reset(new std::mutex());
	}

	std::string request_action(std::string request_type, std::string action, std::string additional_args) {
		const std::string server_url = "play.pokemonshowdown.com";
		const std::string path = "/action.php";
		HTTPClientSession session(server_url);
		std::string params = "act=" + action + additional_args;
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
		std::lock_guard<std::mutex> lock(*data_mutex.get());
		connection.send_msg(message);
	}

	void choose() {}

	void result() {}

	std::string get_username() {
		std::lock_guard<std::mutex> lock(*data_mutex.get());
		return username;
	}

	std::string get_avatar() {
		std::lock_guard<std::mutex> lock(*data_mutex.get());
		return avatar;
	}

	std::string get_chall_str() {
		std::lock_guard<std::mutex> lock(*data_mutex.get());
		return chall_str;
	}

	std::string get_chall_id() {
		std::lock_guard<std::mutex> lock(*data_mutex.get());
		return chall_id;
	}




protected:
	PSConnection connection;
	std::unique_ptr<std::mutex> data_mutex;

	std::string uri = "localhost:8000";

	std::string username;
	std::string avatar;
	std::string chall_str;
	std::string chall_id;
	std::string assertion;

	bool autojoin = false;
	bool is_guest = true;
};

#include <string>
#include <iostream>
#include <cassert>
#include <random>
#include <ctime>
#include <Poco\Net\HTTPClientSession.h>
#include <Poco\Net\HTTPRequest.h>
#include <Poco\Net\HTTPResponse.h>
#include <Poco\Net\WebSocket.h>
using namespace Poco::Net;

std::mt19937 rng(std::time(0));
std::uniform_int_distribution<> rand_id_range(0, 1000);

std::string uri = "localhost:8000";



// create lowercase alpha-numeric + underscore string
std::string random_anum_string(int size) {
	static const std::string possible_chars = "abcdefghijklmnopqrstuvwxyz0123456789_";
	static const std::uniform_int_distribution<> rand_char(0, possible_chars.size() - 1);
	std::string str;
	str.resize(size);
	for (int i = 0; i < size; ++i) {
		str[i] = possible_chars[rand_char(rng)];
	}
	return str;
}

int main() {
	std::printf("Initializing client\n");
	HTTPClientSession cs((SocketAddress)uri);
	std::printf("host: %s port: %d\n", cs.getHost().c_str(), cs.getPort());
	std::string conn_string = std::string("/showdown/") + std::to_string(rand_id_range(rng)) + "/" + random_anum_string(8) + "/websocket";
	std::printf("%s\n", conn_string.c_str());
	HTTPRequest request(HTTPRequest::HTTP_GET, conn_string, HTTPRequest::HTTP_1_1);
	HTTPResponse response;

	std::printf("Client is %sconnected\n", cs.connected() ? "": "not ");

	std::printf("Starting websocket...\n");
	WebSocket ws(cs, request, response);
	//std::printf("Sending message...\n");
	//std::string payload = "/autojoin";
	//ws.sendFrame(payload.data(), (int)payload.size());
	std::string buffer;
	buffer.resize(1024);
	int flags;

	std::printf("Waiting for message...\n");
	int n = ws.receiveFrame((void*)buffer.data(), buffer.size(), flags);
	std::cout << buffer.substr(0,n).c_str() << std::endl;
	//assert(n == payload.size());
	//assert(payload.compare(0, payload.size(), buffer, 0, n) == 0);
	//assert(flags == Poco::Net::WebSocket::FRAME_TEXT);
	std::cin.get();
	return 0;
}

#pragma once
#include <memory>
#include <thread>
#include <string>
#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/Net/NetException.h>
using namespace Poco::Net;
struct PSConnection {
	PSConnection() {}
	PSConnection(std::string uri, std::function<void(std::string)> on_msg = 0) {
		set_on_message(on_msg);
		connect(uri);
	}
	void connect(std::string uri) {
		buffer.resize(4096);
		this->uri = (SocketAddress)uri;
		HTTPClientSession cs(this->uri);
		HTTPRequest request(HTTPRequest::HTTP_GET, conn_string, HTTPRequest::HTTP_1_1);
		HTTPResponse response;
		ws.reset(new WebSocket(cs, request, response));

		ws->setReceiveTimeout(0);
		if (rcv_thread.joinable()) {
			rcv_thread.detach();
		}
		std::thread new_thread(&PSConnection::rcv_message_loop, this);
		rcv_thread.swap(new_thread);
	}
	void set_on_message(std::function<void(std::string)> func) {
		on_message = func;
	}

	void send_msg(std::string &message) {
		ws->sendFrame(message.data(), (int)message.size());
	}

private:
	SocketAddress uri;
	std::unique_ptr<WebSocket> ws;
	const std::string conn_string = std::string("/showdown/websocket");
	std::function<void(std::string)> on_message = 0;
	std::thread rcv_thread;
	std::string buffer;

	void rcv_message_loop() {
		while (!ws) {} // wait until websocket appears
		while (ws) {
				int flags;
				size_t n = 0;
				try {
					n = ws->receiveFrame((void*)buffer.data(), (int)buffer.size(), flags);
					std::string msg = buffer.substr(0, n);
					if (on_message) {
						//temporary mutex lock so that std::out data is readable
						static std::mutex message_mtx;
						std::lock_guard<std::mutex> guard(message_mtx);
						on_message(msg);
					}
				} catch (Poco::Net::NetException &e) {
					std::cout << "WS ERROR: "<< e.what() << std::endl;
					throw e;
				}
		}
	}
};
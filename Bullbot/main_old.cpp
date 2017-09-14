/*
* Copyright (c) 2016, Peter Thorson. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the WebSocket++ Project nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#define ASIO_STANDALONE

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <iostream>
#include <thread>
#include <chrono>

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;


client c;
std::string uri = "ws://localhost:8000";
client::connection_ptr con;


// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.

void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
	std::cout << "msg callback" << std::endl;
	std::cout << "on_message called with hdl: " << hdl.lock().get()
		<< " and message: " << msg->get_payload()
		<< std::endl;


	websocketpp::lib::error_code ec;

	c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
	if (ec) {
		std::cout << "Echo failed because: " << ec.message() << std::endl;
	}
}

void on_open(websocketpp::connection_hdl hdl) {
	std::cout << "opee" << std::endl;
	c.send(hdl, "o", websocketpp::frame::opcode::text);
}

int listen_thread() {
	websocketpp::lib::error_code ec;
	con = c.get_connection(uri, ec);
	while (true) {
		try {
			if (ec) {
				std::cout << "could not create connection because: " << ec.message() << std::endl;
				std::cin.get();
			} else {

				// Note that connect here only requests a connection. No network messages are
				// exchanged until the event loop starts running in the next line.
				c.connect(con);

				// Start the ASIO io_service run loop
				// this will cause a single connection to be made to the server. c.run()
				// will exit when this connection is closed.
				c.run();
			}
		}
		catch (websocketpp::exception const & e) {
			std::cout << "listen failed " << e.what() << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	return 0;
}

int msg_thread() {
	websocketpp::lib::error_code ec;
	std::string msg = "a";
	while (true) {
		con = c.get_connection(uri, ec);
		if (con && !ec) {
			c.send(msg, websocketpp::frame::opcode::TEXT, ec);
			if (ec) {
				std::printf("Error : %s\n", ec.message().c_str());
			}
		} else {
			std::printf("Connection error : %s\n", ec.message().c_str());
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	return 0;
}

int main(int argc, char* argv[]) {
	// Set logging to be pretty verbose (everything except message payloads)
	c.set_access_channels(websocketpp::log::alevel::all);
	//c.clear_access_channels(websocketpp::log::alevel::frame_payload);

	// Initialize ASIO
	c.init_asio();
	
	// Register our message handler
	c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));

	c.set_open_handler(on_open);
	c.set_open_handshake_timeout(0);


	// Create a client endpoint
	std::thread a(listen_thread);
	std::thread b(msg_thread);
	a.join();
	
					
	
}
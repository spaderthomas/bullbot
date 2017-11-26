using namespace Poco::Net;
struct PSConnection {
	PSConnection() {}
  
	PSConnection(std::string uri, std::function<void(std::string)> on_msg = 0) {
		set_on_message(on_msg);
		connect(uri);
	}

	void connect(std::string uri) {
		buffer.resize(4096);
    SocketAddress sockAddr(uri);
		this->uri = sockAddr;
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
		int flags;
		size_t msg_size = 0;
		while (!ws) {} // wait until websocket appears
		while (ws) {
				try {
					msg_size = ws->receiveFrame((void*)buffer.data(), (int)buffer.size(), flags);
					std::string msg = buffer.substr(0, msg_size);
					if (on_message) {
						//temporary mutex lock so that std::out data is readable
						static std::mutex message_mtx;
						mutex_guard guard(message_mtx);
						on_message(msg);
					}
				} catch (Poco::Net::NetException &e) {
					std::cout << "WS ERROR: "<< e.what() << std::endl;
					// throw e;
				}
		}
	}
};

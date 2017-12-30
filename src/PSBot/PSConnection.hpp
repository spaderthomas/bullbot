struct PSConnection {
  // Members
	SocketAddress uri;
	unique_ptr<WebSocket> ws;
	const string conn_string = string("/showdown/websocket");
	function<void(string)> on_message = 0;
	thread rcv_thread;
	string buffer;

  // Constructors
  PSConnection() {}
  
	PSConnection(string uri, function<void(string)> on_msg = 0) {
		set_on_message(on_msg);
		connect(uri);
	}

  // Web interaction functions
	void connect(string uri) {
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
		thread new_thread(&PSConnection::rcv_message_loop, this);
		rcv_thread.swap(new_thread);
	}

	void set_on_message(function<void(string)> func) {
		on_message = func;
	}

	void send_msg(string &message) {
		ws->sendFrame(message.data(), (int)message.size());
	}

	void rcv_message_loop() {
		int flags;
		size_t msg_size = 0;
		while (!ws) {} // wait until websocket appears
		while (ws) {
				try {
					msg_size = ws->receiveFrame((void*)buffer.data(), (int)buffer.size(), flags);
					string msg = buffer.substr(0, msg_size);
					if (on_message) {
						//temporary mutex lock so that out data is readable
						static mutex message_mtx;
						mutex_guard guard(message_mtx);
						on_message(msg);
					}
				} catch (Poco::Net::NetException &e) {
					cout << "POCO socket error:\n" << e.what() << endl;
				}
		}
	}
};

struct BasePSUser {
	//creates a new base user with its own mutex
	BasePSUser() {
		data_mutex_ptr.reset(new mutex());
	}

	PSConnection connection;
	mutex_ptr data_mutex_ptr;

	string uri = "localhost:8000";
	string username;
	string avatar;
	string chall_str;
	string chall_id;
	string assertion;

	bool autojoin = false;
	bool is_guest = true;

	string request_action(string request_type, string action, string additional_args) {
		static const string server_url = "play.pokemonshowdown.com";
		static const string path = "/action.php";
		HTTPClientSession session(server_url);
		string params = "act=" + action + additional_args;
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
		string response;
		try {
			istream& is = session.receiveResponse(resp);
			//copy response to a string
			Poco::StreamCopier::copyToString(is, response);
		} catch (Poco::Net::NetException &e) {
			cout << e.what() << endl;
		}
		return response;
	};

	void send(string message) {
		mutex_guard lock(*data_mutex_ptr.get());
		connection.send_msg(message);
	}

	string get_username() {
		mutex_guard lock(*data_mutex_ptr.get());
		return username;
	}

	string get_avatar() {
		mutex_guard lock(*data_mutex_ptr.get());
		return avatar;
	}

	string get_chall_str() {
		mutex_guard lock(*data_mutex_ptr.get());
		return chall_str;
	}

	string get_chall_id() {
		mutex_guard lock(*data_mutex_ptr.get());
		return chall_id;
	}

};

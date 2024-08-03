#pragma once

#include <vector>
#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "../includes/CgiHandler.hpp"

#define BUFFER_SIZE 3

class WebClient: public Socket {
	enum State {
		READING,
		WRITING,
		COMPLETE,
		ERROR
	};

public:
	WebClient(int accepted_connection);
	~WebClient();

	HttpRequest getRequest() { return _request; };
	void send_data();
	bool process();
	void close();

private:
	State		_state;
	HttpRequest	_request;
	CgiHandler	*_cgi;

	void _processInput();
	void _processCGI();

	WebClient();
};
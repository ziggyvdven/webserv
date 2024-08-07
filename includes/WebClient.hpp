#pragma once

#include <vector>
#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "../includes/CgiHandler.hpp"
#include "../includes/HttpHandler.hpp"

#define BUFFER_SIZE 32

class WebClient: public Socket {
	enum State {
		READING,
		HANDLING_REQUEST,
		COMPLETE,
		ERROR
	};

public:
	WebClient(int accepted_connection, HttpHandler *httpHandler);
	~WebClient();

	HttpRequest getRequest() { return _request; };
	void _sendData(char const *data, size_t data_len);
	bool process();
	void close();

private:
	State				_state;
	HttpRequest			_request;
	std::string			_response;
	HttpHandler			*_httpHandler;
	CgiHandler			*_cgi;
	std::vector<char>	_writeBuffer;

	void _processInput();
	void _processCGI();

	WebClient();
};
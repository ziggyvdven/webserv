#pragma once

#include <ctime>
#include <sys/poll.h>
#include <vector>
#include "Socket.hpp"
#include "HttpRequest.hpp"
#include "../includes/CgiHandler.hpp"
#include "../includes/HttpHandler.hpp"
#include <poll.h>

#define BUFFER_SIZE 32

class WebClient: public Socket {
	enum State {
		READING,
		HANDLING_REQUEST,
		COMPLETE,
		ERROR
	};

public:
	WebClient(int accepted_connection, HttpHandler *httpHandler, struct pollfd *pollFd);
	~WebClient();

	HttpRequest getRequest() { return _request; };
	void _sendData(char const *data, size_t data_len);
	int	getTime() const;
	bool process();
	void close();
	bool	isComplete() const { return _state == COMPLETE; };


	void	_updateTime();
private:
	struct pollfd		*_pollFd;
	State				_state;
	HttpRequest			_request;
	std::string			_response;
	HttpHandler			*_httpHandler;
	CgiHandler			*_cgi;
	std::vector<char>	_writeBuffer;
	std::time_t			_last_update;

	void	_processInput();
	void	_processCGI();

	WebClient();
};
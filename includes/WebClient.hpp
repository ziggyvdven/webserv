#pragma once

#include <vector>
#include "Socket.hpp"
#include "HttpRequest.hpp"

#define BUFFER_SIZE 3

class WebClient: public Socket {
public:
	WebClient(int accepted_connection);
	~WebClient();

	void send_data();
	bool read_data();
	bool process();
private:
	WebClient();
	HttpRequest _request;
	std::vector<char> _read_buffer;
};
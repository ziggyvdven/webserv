#pragma once

#include <vector>
#include "Socket.hpp"
#include "HttpRequest.hpp"

class WebClient: public Socket {
public:
	WebClient(int accepted_connection);
	~WebClient();

	void send_data();
	void read_data();
private:
	WebClient();
	HttpRequest request;
};
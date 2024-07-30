#include "../includes/WebClient.hpp"
#include <sys/socket.h>

WebClient::WebClient(int accepted_connection)
	: Socket(accepted_connection)
{
	std::cout << "[DEBUG] Creating new web clinet" << std::endl;
}

void WebClient::send_data() {
	std::cout << "<Sending data to client>" << std::endl;

}

bool WebClient::read_data() {
	int bytes_read = 0;
	char buffer[BUFFER_SIZE];

	if (!_request.isValid() || _request.isComplete())
	{
		std::cout << "[DEBUG] request is invalid or complete" << std::endl;
		return false;
	}

	bytes_read = recv(_socketFD, buffer, BUFFER_SIZE, 0);
	if (bytes_read <= 0)
	{
		std::cout << "[UNIMPLEMENTED] Web Client disconnected" << std::endl;
		return false;
	}
	_request.parse(buffer, bytes_read);
	return true;
}

WebClient::~WebClient() {}

bool WebClient::process()
{
	if(!read_data()) {
		_request.print_request();
	}

	// Check if it's a Cgi Request

	// Check if it's a get request

	// Handle it accordingly
	
	return true;
}
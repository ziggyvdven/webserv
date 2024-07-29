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


	bytes_read = recv(_socketFD, buffer, BUFFER_SIZE, 0);
	_read_buffer.insert(_read_buffer.end(), buffer, buffer + bytes_read);
	if (bytes_read <= 0)
	{
		return false;
	}
	return true;
}

WebClient::~WebClient() {}

bool WebClient::process()
{
	if(!read_data())
		return false;
	_request.parse(_read_buffer.data(), _read_buffer.size());
	_read_buffer.clear();
	return true;
	// Finished processing
}
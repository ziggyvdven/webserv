#include "../includes/WebClient.hpp"
#include <sys/socket.h>

WebClient::WebClient(int accepted_connection)
	: Socket(accepted_connection), _state(READING)
{
	// std::cout << "[DEBUG] Creating new web clinet" << std::endl;
}

void WebClient::send_data() {
	std::cout << "<Sending data to client>" << std::endl;

}

WebClient::~WebClient() {}

void WebClient::_processInput()
{
	int bytes_read = 0;
	char buffer[BUFFER_SIZE];


	bytes_read = recv(_socketFD, buffer, BUFFER_SIZE, 0);

	if (bytes_read <= 0)
	{
		std::cout << "[UNIMPLEMENTED] Web Client disconnected" << std::endl;
		_state = _request.isComplete() ? COMPLETE : ERROR;
		return ;
	}

	// Parse the request
	_request.parse(buffer, bytes_read);

	if (_request.hasError() || _request.isComplete()) {

		_state = _request.isComplete() ? COMPLETE : ERROR;
		std::cout << "[DEBUG] request is invalid or complete" << std::endl;
	}
}

void WebClient::_processCGI() {
	// Init cgi execution
	std::cout << "[UNIMPLEMENTED] Handling CGI" << std::endl;

	// Time the cgi execution

	//

}

bool WebClient::process()
{
	switch (_state) {
		case READING:
			_processInput();
			break;
		case WRITING:
			std::cout << "[UNIMPLEMENTED] Web Client COMPLETE" << std::endl;
			return false;
		case COMPLETE:
			std::cout << "[UNIMPLEMENTED] Web Client COMPLETE" << std::endl;
			return false;
		case ERROR:
			std::cout << "[UNIMPLEMENTED] Web Client ERROR" << std::endl;
			return false;

	}

	return !(_state == COMPLETE || _state == ERROR);
}

#include "../includes/WebClient.hpp"
#include <sys/socket.h>

WebClient::WebClient(int accepted_connection, HttpHandler* httpHandler)
	: Socket(accepted_connection), _state(READING), _httpHandler(httpHandler)
{
}

void WebClient::_sendData(char const *data, size_t data_len) {
	size_t	rtn = 0;
	size_t	idx = 0;
	size_t	chunk_size = 255;
	size_t	send_size = 0;

	while (idx < data_len)
	{
		send_size = data_len - idx < chunk_size ? data_len - idx : chunk_size;
		rtn = send(_socketFD, data + idx, send_size, 0);
		idx += rtn;
	}

}

WebClient::~WebClient() {}

void WebClient::_processInput()
{
	int bytes_read = 0;
	char buffer[BUFFER_SIZE];


	bytes_read = recv(_socketFD, buffer, BUFFER_SIZE, 0);

	if (bytes_read <= 0)
	{
		_state = HANDLING_REQUEST;
		return ;
	}

	// Parse the request
	_request.parse(buffer, bytes_read);

	if (_request.hasError() || _request.isComplete()) {

		_state = HANDLING_REQUEST;
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
		case HANDLING_REQUEST:
			_response = _httpHandler->buildResponse(_request);
			_sendData(_response.data(), _response.size());
			_state = COMPLETE;
			return false;
		case COMPLETE:
			std::cout << "[UNIMPLEMENTED] Web Client COMPLETE" << std::endl;
			break;
		case ERROR:
			std::cout << "[UNIMPLEMENTED] Web Client ERROR" << std::endl;
			return false;
	}



	return !(_state == COMPLETE);
}

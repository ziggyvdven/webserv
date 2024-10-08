#include "../includes/TcpListener.hpp"

TcpListener::TcpListener(std::string const host, unsigned int const port)
	: Socket(host, port)
{

}
TcpListener::~TcpListener() {

}

int accept_connection();
int	TcpListener::createListeningSocket(void)
{
	int	option = 1;

	// Create a socket
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD < 0)
	{
		return (_errorMessage("socket() failed"));
	}

	// Allow socket to reuse local address
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
	{
		return (_errorMessage("setsockopt() failed"));
	}

	// Set socket to be nonblocking
	if (fcntl(_socketFD, F_SETFL, O_NONBLOCK) < 0)
	{
		return (_errorMessage("fcntl() failed"));
	}

	// Identify a socket (bind the ip address and port)
	if (bind(_socketFD, reinterpret_cast<struct sockaddr *>(&_address), sizeof(_address)) < 0)
	{
		return (_errorMessage("bind() failed"));
	}

	// Listen to accepting incoming connections
	if (listen(_socketFD, SOMAXCONN) < 0)
	{
		return (_errorMessage("listen() failed"));
	}

	return (0);
}
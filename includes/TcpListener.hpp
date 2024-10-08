#pragma once 
#include "../includes/Socket.hpp"

class TcpListener: public Socket{
private:

public:
	TcpListener(std::string const host, unsigned int const port);
	~TcpListener();
	// TcpListener(TcpListener const &other);
	// TcpListener& operator= (TcpListener const &other);

	int	createListeningSocket(void);
	int accept_connection();
};
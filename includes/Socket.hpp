#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <unistd.h>
# include <iostream>
# include <string>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <fcntl.h>

class Socket
{
private:

	struct sockaddr_in	_address;
	std::string const	_host;
	unsigned int const	_port;
	int					_socketFD;

	std::string			_root;

	int					_errorMessage(std::string const msg) const;

public:

	Socket(std::string const host, unsigned int const port);
	~Socket();

	int					createSocket(void);
	int					getSocketFD(void) const;

};

#endif

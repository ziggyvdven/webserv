#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <fstream>
# include <iostream>
# include <sstream>
# include <string>
# include <poll.h>
# include <sys/socket.h>
# include <unistd.h>
# include <csignal>
# include <vector>
# include "HttpHandler.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "Socket.hpp"
# include "Config.hpp"

class HttpHandler;

class WebServer
{
private:

	struct pollfd				_fds[FD_SETSIZE];
	int							_nfds;
	std::vector<Socket>			_socketList;
	size_t const				_socketListSize;
	Config&						_config;

	std::vector<unsigned char>	_request;
	std::string					_response;

	static bool					_serverOn;
	bool						_closeConnection;

	bool						_isListeningSocket(int fd) const;
	void						_acceptConnection(int fd);
	void						_compressFdsArray(void);
	bool						_readData(int socket);
	void						_sendData(int socket, const char* str, size_t len) const;

	static void					_sighandler(int signum);

public:

	WebServer(std::vector<Socket> socketList, Config &config);
	~WebServer();

	int	init(void);
	int	run(void);

	void						cleanUpSockets(void) const;
	int							getNFds(void) const;

	void						printFdsArray(void) const;

};

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:18:55 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/23 20:06:04 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

	std::string					_request;
	std::string					_response;

	bool						_isListeningSocket(int fd) const;
	void						_acceptConnection(int fd);
	void						_compressFdsArray(void);
	bool						_readData(int socket);
	void						_sendData(int socket, const char* str, size_t len) const;

public:

	WebServer(std::vector<Socket> socketList, Config &config);
	~WebServer();

	int	init(void);
	int	run(void);

	void						cleanUpSockets(void) const;
	int							getNFds(void) const;
	Config&						getConfig(void);
	
};

#endif

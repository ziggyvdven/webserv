/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmehour <kmehour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:18:55 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/13 18:51:59 by kmehour          ###   ########.fr       */
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

class WebServer
{
private:

	struct pollfd		_fds[FD_SETSIZE];
	int					_nfds;
	std::vector<Socket>	_socketList;
	size_t const		_socketListSize;

	std::string			_request;
	std::string			_response;

	bool				_isListeningSocket(int fd) const;
	void				_acceptConnection(int fd);
	void				_cleanUpSockets(void) const;
	void				_compressFdsArray(void);
	bool				_readData(int socket);
	void				_sendData(int socket, const char* str, size_t len) const;

	void				_printRequest(void);

public:

	WebServer(std::vector<Socket> socketList);
	~WebServer();

	int	init(void);
	int	run(void);

};

#endif
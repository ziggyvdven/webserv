/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:18:55 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/09 18:04:18 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <fstream>
# include <iostream>
# include <sstream>
# include <poll.h>
# include <sys/socket.h>
# include <unistd.h>
# include <vector>
# include "HttpHandler.hpp"
# include "Socket.hpp"

class WebServer
{
private:

	struct pollfd		_fds[FD_SETSIZE];
	// int 				_listeningSocket;
	int					_nfds;

	// int					_errorMessage(std::string const msg) const;

	void				_acceptConnections(void);
	void				_cleanUpSockets(void) const;
	void				_compressFdsArray(void);
	std::string const	_getContentType(std::string const &path);
	std::string const	_onMessageReceived(const char* msg);

	void				_sendData(int socket, const char* str, size_t len) const;

public:

	WebServer(void);
	~WebServer();

	int	init(std::vector<Socket> socketList);
	int	run(void);

};

#endif

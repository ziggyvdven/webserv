/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:18:55 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/05 15:12:15 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <fstream>
# include <iostream>
# include <sstream>
# include <netinet/in.h>
# include <poll.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <fcntl.h>
# include <unistd.h>
# include <vector>
# include "HttpHandler.hpp"

# define PORT			8080

class WebServer
{
private:

	struct sockaddr_in	_address;
	struct pollfd		_fds[FD_SETSIZE];
	int 				_listeningSocket;
	bool				_listeningSocketCreated;
	int					_nfds;

	int					_errorMessage(std::string const msg) const;

	void				_acceptConnections(void);
	void				_cleanUpSockets(void) const;
	void				_compressFdsArray(void);
	std::string const	_getContentType(std::string const &path);
	std::string const	_onMessageReceived(const char* msg);

	void				_sendData(int socket, const char* str, size_t len) const;

public:

	WebServer(void);
	~WebServer();

	int	init(void);
	int	run(void);

};

#endif

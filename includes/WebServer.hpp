/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:18:55 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/10 15:06:25 by oroy             ###   ########.fr       */
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

# define BUFFER_SIZE 256

class WebServer
{
private:

	struct pollfd		_fds[FD_SETSIZE];
	int					_nfds;
	std::vector<Socket>	_socketList;
	size_t const		_socketListSize;

	bool				_findInSocketList(int fd) const;
	void				_acceptConnections(int fd);
	void				_cleanUpSockets(void) const;
	void				_compressFdsArray(void);
	void				_sendData(int socket, const char* str, size_t len) const;

public:

	WebServer(std::vector<Socket> socketList);
	~WebServer();

	int	init(void);
	int	run(void);

};

#endif

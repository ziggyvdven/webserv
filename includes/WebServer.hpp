/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmehour <kmehour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:18:55 by olivierroy        #+#    #+#             */
/*   Updated: 2024/08/08 17:35:13 by kmehour          ###   ########.fr       */
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
# include "TcpListener.hpp"
# include "WebClient.hpp"
# include "Config.hpp"

#define CLIENT_TIMEOUT 3

class HttpHandler;

class WebServer
{
private:

	struct pollfd				_fds[FD_SETSIZE];
	int							_nfds;
	std::vector<TcpListener>	_listeners_list;
	std::vector<WebClient>		_clients_list;
	Config&						_config;
	std::vector<unsigned char>	_request;
	std::string					_response;
	HttpHandler					_httpHandler;

	bool						_isListeningSocket(int fd);
	void						_handleNewConnections();
	void						_processClients();
	void						_removeClient(WebClient *client_ptr);
	WebClient*					_getClient(int socketFd);
	static bool					_serverOn;
	bool						_closeConnection;

	bool						_isListeningSocket(int fd) const;
	void						_acceptConnection(int fd);
	void						_compressFdsArray(void);

	static void					_sighandler(int signum);

public:
	WebServer(std::vector<TcpListener> socketList, Config &config);
	~WebServer();

	int	init(void);
	int	run(void);

	void						cleanUpSockets(void) const;
	int							getNFds(void) const;
	Config&						getConfig(void);

	void						printFdsArray(void) const;

};

#endif

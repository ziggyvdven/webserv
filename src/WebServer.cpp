/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:20:26 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/09 18:01:20 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"

WebServer::WebServer(void) : _nfds(0)
{
	memset(_fds, 0, sizeof(_fds));
}

WebServer::~WebServer()
{

}

int	WebServer::init(std::vector<Socket> socketList)
{
	size_t	vectorSize = socketList.size();

	for (size_t i = 0; i < vectorSize; ++i)
	{
		if (socketList[i].createSocket() < 0)
		{
			_cleanUpSockets();
			return (-1);
		}
		// Add the listening socket in our fd set
		_fds[i].fd = socketList[i].getSocketFD();
		_fds[i].events = POLLIN;
		_nfds++;
	}
	return (0);
}

int	WebServer::run(void)
{
	HttpHandler	http;
	char		buffer[8000];
	int			current_fds_size = 0;
	ssize_t		read_size = 0;
	std::string	str;

	while (true)
	{
		// Poll process. Checks the fds array to see if some file descriptors are ready for I/O
		if (poll(_fds, _nfds, 0) < 0)
		{
			std::cerr << "poll() failed" << std::endl;
		}

		// Loop through the fds that returned POLLIN and check if it's the listening or active socket
		current_fds_size = _nfds;
		for (int i = 0; i < current_fds_size; i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _listeningSocket)
				{
					// This is a listening socket
					// Process incoming connections and add to fds array
					_acceptConnections();
				}
				else
				{
					// This is an accepting socket. Do recv/send loop
					memset(buffer, 0, sizeof(buffer));
					read_size = recv(_fds[i].fd, buffer, 8000 + 1, 0);
					if (read_size > 0)
					{
						// std::cout << "Request length: " << read_size << std::endl;
						std::cout << buffer << std::endl;
						// Send HTTP Response
						str = http.handleRequest(buffer);
						// str = _onMessageReceived(buffer);
						_sendData(_fds[i].fd, str.c_str(), str.size() + 1);
						// if (send(_fds[i].fd, str.c_str(), str.size() + 1, 0) < 0)
						// {
						// 	std::cerr << "send() failed" << std::endl;
						// }
						std::cout << "\n------------------ Message sent -------------------\n\n";
					}
					// Close Accepting Socket
					close(_fds[i].fd);
					_fds[i].fd = -1;
					_compressFdsArray();
				}
			}
		}
	}
	_cleanUpSockets();
	return (0);
}

void	WebServer::_sendData(int socket, const char* str, size_t len) const
{
	ssize_t	rtn = 0;
	size_t	idx = 0;

	while (idx < len)
	{
		rtn = send(socket, str + idx, 256, 0);
		idx += rtn;
	}
}

void	WebServer::_acceptConnections(void)
{
	int	new_fd = 0;

	while (true)
	{
		new_fd = accept(_listeningSocket, NULL, NULL);
		if (new_fd < 0)
		{
			// If errno is EWOULDBLOCK, then all connections have been accepted
			if (errno != EWOULDBLOCK)
			{
				std::cerr << "accept() failed" << std::endl;
			}
			return ;
		}
		// std::cout << "New incoming connection - " << new_fd << std::endl;
		_fds[_nfds].fd = new_fd;
		_fds[_nfds].events = POLLIN;
		_nfds++;
	}
}

void	WebServer::_compressFdsArray(void)
{
	for (int i = 0; i < _nfds; i++)
	{
		std::cout << i << std::endl;
		if (_fds[i].fd == -1)
		{
			for(int j = i; j < _nfds - 1; j++)
			{
				_fds[j].fd = _fds[j + 1].fd;
			}
			i--;
			_nfds--;
		}
	}
}

void	WebServer::_cleanUpSockets(void) const
{
	for (int i = 0; i < _nfds; i++)
	{
		if(_fds[i].fd >= 0)
			close(_fds[i].fd);
	}
}

// int	WebServer::_errorMessage(std::string const msg) const
// {
// 	std::cerr << msg << std::endl;
// 	// if (_listeningSocketCreated)
// 	// {
// 	// 	close (_listeningSocket);
// 	// 	std::cout << "Listening Socket Closed" << std::endl;
// 	// }
// 	return (1);
// }

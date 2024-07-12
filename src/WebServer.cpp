/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:20:26 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/11 21:14:55 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"

WebServer::WebServer(std::vector<Socket> socketList) : _nfds(0), \
_socketList(socketList), _socketListSize(socketList.size())
{
	memset(_fds, 0, sizeof(_fds));
}

WebServer::~WebServer()
{

}

int	WebServer::init(void)
{
	for (size_t i = 0; i < _socketListSize; ++i)
	{
		if (_socketList[i].createSocket() < 0)
		{
			_cleanUpSockets();
			return (-1);
		}
		// Add the listening socket in our fd set
		_fds[i].fd = _socketList[i].getSocketFD();
		_fds[i].events = POLLIN;
		_nfds++;
	}
	return (0);
}

int	WebServer::run(void)
{
	HttpHandler	http;
	// char		buffer[200000 + 1];
	int			current_fds_size = 0;
	// ssize_t		read_size = 0;
	// std::string	response;

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
				if (_findInSocketList(_fds[i].fd))
				{
					// This is a listening socket
					// Process incoming connections and add to fds array
					_acceptConnections(_fds[i].fd);
				}
				else
				{
					// This is an accepting socket. Do recv/send loop
					// memset(buffer, 0, sizeof(buffer));
					// read_size = recv(_fds[i].fd, buffer, 200000, 0);
					// if (read_size > 0)
					if (_readData(_fds[i].fd))
					{
						std::cout << _request << std::endl;
						// Send HTTP Response
						_response = http.handleRequest(_request);
						// response = "HTTP/1.1 100 Continue\r\n\r\n";
						_sendData(_fds[i].fd, _response.c_str(), _response.size() + 1);
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

bool	WebServer::_findInSocketList(int fd) const
{
	for (size_t i = 0; i < _socketListSize; ++i)
	{
		if (fd == _socketList[i].getSocketFD())
			return (true);
	}
	return (false);
}

void	WebServer::_acceptConnections(int fd)
{
	int	new_fd = 0;

	// while (true)
	// {
		new_fd = accept(fd, NULL, NULL);
		if (new_fd < 0)
		{
			// If errno is EWOULDBLOCK, then all connections have been accepted
			if (errno != EWOULDBLOCK)
			{
				std::cerr << "accept() failed" << std::endl;
			}
			return ;
		}
		_fds[_nfds].fd = new_fd;
		_fds[_nfds].events = POLLIN;
		_nfds++;
	// }
}

bool	WebServer::_readData(int socket)
{
	char		buffer[256 + 1];
	std::string	buffer_str;
	std::string	request;
	ssize_t		rtn = 0;
	
	memset(buffer, 0, sizeof (buffer));
	while ((rtn = recv(socket, buffer, 256, 0)) > 0)
	{
		buffer_str = buffer;
		request += buffer_str.substr(0, rtn);
		memset(buffer, 0, rtn);
	}
	_request = request;
	if (rtn == 0)
	{
		return (false);
	}
	return (true);
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

void	WebServer::_compressFdsArray(void)
{
	for (int i = 0; i < _nfds; i++)
	{
		// std::cout << i << std::endl;
		if (_fds[i].fd == -1)
		{
			for(int j = i; j < _nfds - 1; j++)
			{
				_fds[j] = _fds[j + 1];
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

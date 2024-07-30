/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmehour <kmehour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:20:26 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/29 20:18:41 by kmehour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/WebServer.hpp"
#include <cstddef>
#include <ios>
#include <type_traits>

WebServer::WebServer(std::vector<TcpListener> socketList, Config &conf) : _nfds(0), \
_listeners_list(socketList), _config(conf)
{
	memset(_fds, 0, sizeof(_fds));
}

WebServer::~WebServer()
{

}

int	WebServer::init(void)
{
	for (size_t i = 0; i < _listeners_list.size(); i++)
	{
		if (_listeners_list[i].createListeningSocket() < 0)
		{
			cleanUpSockets();
			return (-1);
		}
		// Add the listening socket in our fd set
		_fds[i].fd = _listeners_list[i].getSocketFD();
		_fds[i].events = POLLIN;
		_nfds++;
	}
	return (0);
}

int	WebServer::run(void)
{
	HttpHandler	http(*this, _config);
	int			current_fds_size;
	WebClient	*client_ptr;
	TcpListener	*listener_ptr;

	while (true)
	{
		// Poll process. Checks the fds array to see if some file descriptors are ready for I/O
		if (poll(_fds, _nfds, 0) < 0)
		{
			std::cerr << "poll() failed" << std::endl;
		}

		// Loop through the fds that returned POLLIN and check if it's the listening or active socket
		current_fds_size = _nfds;
		// std::cout << "_nfds: " << _nfds << std::endl;
		for (int i = 0; i < current_fds_size; i++)
		{
			// std::cout << "- [Debug] fd: " << i << std::endl;
			if (_fds[i].revents & POLLIN)
			{
				if ((listener_ptr = _getListeningSocket(_fds[i].fd)))
				{
					// This is a listening socket
					// Process incoming connections and add to fds array
					_acceptConnection(_fds[i].fd);
					break;
				}
				
				client_ptr = _getClientSocket(_fds[i].fd);
			
				if (client_ptr->process())
				{
					continue;
				}
				
				// This is an accepting socket. Do recv/send loop
				// if (_readData(_fds[i].fd))
				// {
				// 	// Send HTTP Response
				// 	HttpRequest request;
				// 	_request.clear();
					_response = http.buildResponse(client_ptr->getRequest());
					_sendData(_fds[i].fd, _response.data(), _response.size());
				// 	std::cout << "\n------------------ Message sent -------------------\n\n";
				// }
				// Close Accepting Socket
				close(_fds[i].fd);
				_fds[i].fd = -1;
				_compressFdsArray();
			
				
			}
		}
	}
	cleanUpSockets();
	return (0);
}

TcpListener*	WebServer::_getListeningSocket(int fd)
{
	for (size_t i = 0; i < _listeners_list.size(); ++i)
	{
		if (fd == _listeners_list[i].getSocketFD())
			return (&_listeners_list[i]);
	}
	return (nullptr);
}

WebClient*	WebServer::_getClientSocket(int fd)
{
	for (size_t i = 0; i < _clients_list.size(); ++i)
	{
		if (fd == _clients_list[i].getSocketFD())
			return (&_clients_list[i]);
	}
	return (nullptr);
}

void	WebServer::_acceptConnection(int fd)
{
	int	new_fd = 0;

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
	std::cout << "[DEBUG] Accepting connection to fd " << fd << std::endl;
	_fds[_nfds].fd = new_fd;
	_fds[_nfds].events = POLLIN;
	_nfds++;
	
	WebClient new_client(new_fd);
	_clients_list.push_back(new_client);
}

bool	WebServer::_readData(int socket)
{
	char		buffer[255 + 1];
	ssize_t		rtn = 0;

	memset(buffer, 0, sizeof (buffer));
 	while ((rtn = recv(socket, buffer, 255, 0)) > 0)
	{
		_request.insert(_request.end(), buffer, buffer + rtn);
		memset(buffer, 0, rtn);
	}
	if (rtn == 0)
	{
		return (false);
	}
	return (true);
}

void	WebServer::_sendData(int socket, const char* str, size_t len) const
{
	size_t	rtn = 0;
	size_t	idx = 0;
	size_t	chunk_size = 255;
	size_t	send_size = 0;

	while (idx < len)
	{
		send_size = len - idx < chunk_size ? len - idx : chunk_size;
		rtn = send(socket, str + idx, send_size, 0);
		idx += rtn;
	}
}

void	WebServer::_compressFdsArray(void)
{
	for (int i = 0; i < _nfds; i++)
	{
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

void	WebServer::cleanUpSockets(void) const
{
	for (int i = 0; i < _nfds; i++)
	{
		if(_fds[i].fd >= 0)
			close(_fds[i].fd);
	}
}

Config& WebServer::getConfig(void){
	return (_config);
}

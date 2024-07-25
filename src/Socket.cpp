/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 15:53:01 by oroy              #+#    #+#             */
/*   Updated: 2024/07/24 19:20:22 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Socket.hpp"

Socket::Socket(std::string const host, unsigned int const port) : _host(host), _port(port), _socketFD(-1)
{
	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	// if (inet_pton(AF_INET, _host.c_str(), &_address.sin_addr) <= 0)
	// {
	// 	perror("inet_pton failed");
    // 	exit(1);
	// }
	return ;
}

Socket::~Socket()
{
	return ;
}

int	Socket::createSocket(void)
{
	int	option = 1;

	// Create a socket
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD < 0)
	{
		return (_errorMessage("socket() failed"));
	}

	// Allow socket to reuse local address
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
	{
		return (_errorMessage("setsockopt() failed"));
	}

	// Set socket to be nonblocking
	if (fcntl(_socketFD, F_SETFL, O_NONBLOCK) < 0)
	{
		return (_errorMessage("fcntl() failed"));
	}

	// Identify a socket (bind the ip address and port)
	if (bind(_socketFD, reinterpret_cast<struct sockaddr *>(&_address), sizeof(_address)) < 0)
	{
		return (_errorMessage("bind() failed"));
	}

	// Listen to accepting incoming connections
	if (listen(_socketFD, SOMAXCONN) < 0)
	{
		return (_errorMessage("listen() failed"));
	}

	return (0);
}

int	Socket::getSocketFD(void) const
{
	return (_socketFD);
}

int	Socket::_errorMessage(std::string const msg) const
{
	std::cerr << msg << std::endl;
	if (_socketFD >= 0)
	{
		close (_socketFD);
	}
	return (-1);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 18:56:37 by oroy              #+#    #+#             */
/*   Updated: 2024/07/10 13:47:26 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <unistd.h>
# include <iostream>
# include <string>
# include <netinet/in.h>
# include <sys/socket.h>
# include <fcntl.h>

class Socket
{
private:

	struct sockaddr_in	_address;
	std::string const	_host;
	unsigned int const	_port;
	int					_socketFD;

	std::string			_root;

	int					_errorMessage(std::string const msg) const;

public:

	Socket(std::string const host, unsigned int const port);
	~Socket();

	int					createSocket(void);
	int					getSocketFD(void) const;

};

#endif

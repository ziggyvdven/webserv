/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmehour <kmehour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/08 18:56:37 by oroy              #+#    #+#             */
/*   Updated: 2024/08/01 13:31:03 by kmehour          ###   ########.fr       */
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
	Socket() {};

protected:
	Socket(int socket_fd);
	Socket(std::string const host, unsigned int const port);
	Socket(Socket const &);
	Socket& operator= (Socket const &);
	~Socket();

	struct sockaddr_in	_address;
	std::string			_host;
	unsigned int		_port;
	int					_socketFD;
	std::string			_root;


public:
	int					getSocketFD(void) const;
	int					_errorMessage(std::string const msg) const;
	void				close_connection();
};

#endif

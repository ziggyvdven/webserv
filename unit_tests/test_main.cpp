#include <ios>
#include <iostream>
#include <unistd.h>
#include <sys/fcntl.h>
#include "../includes/utils.hpp"
#include "../includes/Socket.hpp"
#include "../includes/HttpRequest.hpp"

#include <poll.h>


struct Connection{
	int fd;
	HttpRequest request;
};

std::vector<Connection> _connections;
int _nfds = 0;
struct pollfd _fds[1024];


void acceptConnection(int fd)
{
	int new_fd;
	new_fd = accept(fd, NULL, NULL);

	Connection new_connection;
	// Error Handling

	_connections.push_back(new_connection) ;
	_fds[_nfds].fd = new_fd;
	_fds[_nfds].events = POLLIN;
	++_nfds;
}

int main()
{
	Socket server_socket("0.0.0.0", 4240);

	server_socket.createSocket();
	std::cout << "Socket created: " << server_socket.getSocketFD() << std::endl;

	// Creating a server fd
	_fds[_nfds].fd = server_socket.getSocketFD();
	_fds[_nfds].events = POLLIN;
	++_nfds;

	// Create a quick poll loop
	while(true)
	{
		poll(_fds, _nfds, 0);

		// Accep connections if there is any
		for (int i = 0; i < )

		// Loop through actual connections, and handle theme
		
		// for (int i = 0; i < _nfds; i++)
		// {
		// 	if (! (_fds[i].revents & POLLIN)) {
		// 		continue;
		// 	}
		// 	int fd = _fds[i].fd;

		// 	// If it's the server
		// 	if (_fds[i].fd == server_socket.getSocketFD())
		// 	{
		// 		std::cout << "Server expecting a new connection" << std::endl;
		// 		acceptConnection(server_socket.getSocketFD());
		// 		break;
		// 	}
			
		// 	char buf_read[1024];
		// 	int bytes_read = 0;

		// 	bytes_read = recv(fd, buf_read, 1024, 0);
		// 	if (bytes_read <= 0)
		// 	{
		// 		std::cout << "Client disconnect" << std::endl;
		// 		exit(0);
		// 	}
		// 	std::cout << "Connection is sending some data" << std::endl;
		// 	std::string &buf_con ;
		// 	buf_con.append(buf_read, bytes_read);
			
		// 	std::cout << "\nCurrent buffer : \n\n" << buf_con << std::endl;
		// }
		// sleep(1);
	}
}

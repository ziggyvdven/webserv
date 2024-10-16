#include "../includes/WebServer.hpp"
#include <cstddef>
#include <ios>
#include <type_traits>

WebServer::WebServer(std::vector<Socket> socketList, Config &conf) : _nfds(0), \
_socketList(socketList), _socketListSize(socketList.size()), _config(conf)
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
			cleanUpSockets();
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
	HttpHandler	http(_config);
	int			current_fds_size;

	signal(SIGINT, _sighandler);
	while (_serverOn)
	{
		// Poll process. Checks the fds array to see if some file descriptors are ready for I/O
		if (poll(_fds, _nfds, 0) < 0)
			continue;
		// Loop through the fds that returned POLLIN and check if it's the listening or active socket
		current_fds_size = _nfds;
		for (int i = 0; i < current_fds_size; i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_isListeningSocket(_fds[i].fd))
				{
					// This is a listening socket
					// Process incoming connections and add to fds array
					_acceptConnection(_fds[i].fd);
				}
				else
				{
					// This is an accepting socket. Do recv/send loop
					if (_readData(_fds[i].fd))
					{
						// Send HTTP Response
						HttpRequest request(_request);
						HttpResponse response;
						_request.clear();
						http.buildResponse(request, response);
						_response = response.getResponse();
						_sendData(_fds[i].fd, _response.data(), _response.size());
					}
					// printFdsArray();
					// Close Accepting Socket
					close(_fds[i].fd);
					_fds[i].fd = -1;
					_compressFdsArray();
				}
			}
		}
	}
	cleanUpSockets();
	return (0);
}

bool	WebServer::_isListeningSocket(int fd) const
{
	for (size_t i = 0; i < _socketListSize; ++i)
	{
		if (fd == _socketList[i].getSocketFD())
			return (true);
	}
	return (false);
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
	_fds[_nfds].fd = new_fd;
	_fds[_nfds].events = POLLIN;
	_nfds++;
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
			memset(&_fds[_nfds - 1], 0, sizeof (struct pollfd));
			i--;
			_nfds--;
		}
	}
	// printFdsArray();
}

void	WebServer::printFdsArray(void) const
{
	for (int i = 0; i < _nfds; ++i)
	{
		std::cout << "====== FD idx " << i << " ======" << std::endl;
		std::cout << "FD = " << _fds[i].fd << std::endl;
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

void	WebServer::_sighandler(int signum)
{
	(void) signum;
	_serverOn = false;
}

bool	WebServer::_serverOn = true;

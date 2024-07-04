/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 20:20:26 by olivierroy        #+#    #+#             */
/*   Updated: 2024/07/04 18:34:12 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"

WebServer::WebServer(void)
{
	mimeTypes[".html"] = "text/html";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".avif"] = "image/avif";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".gif"] = "image/gif";
	_listeningSocketCreated = false;
	_nfds = 1;
}

WebServer::~WebServer()
{

}

int	WebServer::init(void)
{
	int	opt = 1;

	// Create a socket
	_listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listeningSocket < 0)
	{
		return (_errorMessage("socket() failed"));
	}
	_listeningSocketCreated = true;

	// Allow socket to reuse local address
	if (setsockopt(_listeningSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		return (_errorMessage("setsockopt() failed"));
	}

	// Set socket to be nonblocking
	if (fcntl(_listeningSocket, F_SETFL, O_NONBLOCK) < 0)
	{
		return (_errorMessage("fcntl() failed"));
	}

	// Identify a socket (bind the ip address and port)
	memset(&_address, 0, sizeof(_address));
	_address.sin_family = AF_INET;
	_address.sin_port = htons(PORT);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_listeningSocket, reinterpret_cast<struct sockaddr *>(&_address), sizeof(_address)) < 0)
	{
		return (_errorMessage("bind() failed"));
	}
	
	// Listen to accepting incoming connections
	if (listen(_listeningSocket, SOMAXCONN) < 0)
	{
		return (_errorMessage("listen() failed"));
	}

	// Add the listening socket in our fd set
	memset(_fds, 0, sizeof(_fds));
	_fds[0].fd = _listeningSocket;
	_fds[0].events = POLLIN;

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
			return (_errorMessage("poll() failed"));
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

void	WebServer::_cleanUpSockets(void) const
{
	for (int i = 0; i < _nfds; i++)
	{
		if(_fds[i].fd >= 0)
			close(_fds[i].fd);
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

int	WebServer::_errorMessage(std::string const msg) const
{
	std::cerr << msg << std::endl;
	if (_listeningSocketCreated)
	{
		close (_listeningSocket);
		std::cout << "Listening Socket Closed" << std::endl;
	}
	return (1);
}

// std::string const	WebServer::_getContentType(std::string const &path)
// {
// 	size_t dotPos = path.rfind('.');
// 	if (dotPos != std::string::npos) {
// 		std::string ext = path.substr(dotPos);
// 		std::unordered_map<std::string, std::string>::const_iterator it = mimeTypes.find(ext);
// 		if (it != mimeTypes.end()) {
// 			return it->second;
// 		}
// 	}
// 	return "application/octet-stream"; // Default binary type
// }

// std::string const	WebServer::_onMessageReceived(const char* msg)
// {
// 	// GET /index.html HTTP/1.1

// 	// Parse out the document requested
// 	std::istringstream	iss(msg);
// 	std::vector<std::string>	parsed((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

// 	std::string		content = "<h1>404 Not Found</h1>";
// 	std::string		htmlFile = "/index.html";
// 	std::string		contentType = "text/html";
// 	int				errorCode = 404;

// 	if (parsed.size() >= 3 && parsed[0] == "GET")
// 	{
// 		htmlFile = parsed[1];
// 		if (htmlFile == "/")
// 		{
// 			htmlFile = "/index.html";
// 		}
// 	}

// 	// Open the document in the local file system
// 	std::ifstream	f("./wwwroot" + htmlFile, std::ios::binary);

// 	if (f.good())
// 	{
// 		// std::string	str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
// 		// content = str;
// 		std::vector<char> buffer((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
// 		content = std::string(buffer.begin(), buffer.end());
// 		contentType = _getContentType(htmlFile);
// 		errorCode = 200;
// 	}

// 	f.close();

// 	// Write the document back to the client
// 	std::ostringstream	oss;
// 	oss << "HTTP/1.1 " << errorCode << " OK\r\n";
// 	oss << "Cache-Control: no-cache, private\r\n";
// 	oss << "Content-Type: " << contentType << "\r\n";
// 	oss << "Content-Length: " << content.size() << "\r\n";
// 	oss << "\r\n";
// 	oss << content;

// 	std::string	output = oss.str();
// 	return (output);
// }

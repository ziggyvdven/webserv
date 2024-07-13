/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmehour <kmehour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:30:55 by oroy              #+#    #+#             */
/*   Updated: 2024/07/13 15:43:42y kmehour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HttpHandler.hpp"
#include "../includes/HttpRequest.hpp"

HttpHandler::HttpHandler(void)
{

}

HttpHandler::~HttpHandler()
{

}

/*	Functions	************************************************************* */

// std::string const	HttpHandler::handleRequest(HttpRequest const &request)
// {
// 	if (request.target() == "/cgi-bin/upload.py")
// 		_execCgiScript();

// 	_buildResponse();
// 	return (_response);
// }

std::string const &	HttpHandler::buildResponse(HttpRequest const &request)
{
	// 1 - Check if the request is valid
	// if (!_req.isValid)
	// {
	// 	_statusCode = 400;
	// 	_target = config.getErrorPage();	// 40x.html
	// }

	// 2 - Make sure the target doesn't try to go to a higher directory  
	_target = request.target();
	_parseTarget();

	// std::ifstream		f(config.getRoot() + _target, std::ios::binary);
	std::ifstream		f("./data/www" + _target, std::ios::binary);
	std::string			content = request.raw();
	int					errorCode = 404;

	if (f.good())
	{
		std::string	str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
		content = str;
		errorCode = 200;
	}
	f.close();

	std::ostringstream	oss;

	oss << "HTTP/1.1 " << errorCode << " OK\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	// oss << "Content-Type: " << contentType << "\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	oss << "\r\n";
	oss << content;

	_response = oss.str();
	return _response;
}

void	HttpHandler::_parseTarget(void)
{
	std::string	needle = "../";
	size_t		found = 0;

	while ((found = _target.find(needle, found)) != std::string::npos)
	{
		_target.erase(found, needle.size());
	}
}

// void	HttpHandler::_execCgiScript(void) const
// {
// 	std::vector<char const *>	argv;
// 	std::vector<char const *>	envp;
// 	const char					*python_path = "/usr/bin/python";
// 	pid_t						process_id;

// 	argv.push_back(python_path);
// 	argv.push_back("/Users/oroy/Documents/cursus42/webserv/cgi-bin/upload.py");
// 	argv.push_back(NULL);

// 	process_id = fork();
// 	if (process_id < 0)
// 		perror ("fork() failed");
// 	else if (process_id == 0)
// 	{
// 		execve (python_path, const_cast<char * const *>(argv.data()), NULL);
// 		perror ("execve() failed");
// 		exit (EXIT_FAILURE);
// 	}
// }

/*	Utils	***************************************************************** */

// char const	*HttpHandler::_getScriptName(void) const
// {
// 	size_t	i;
	
// 	i = _target.find('/');
// 	i = _target.find('/', i);

// 	return (&_target[i + 1]);
// }
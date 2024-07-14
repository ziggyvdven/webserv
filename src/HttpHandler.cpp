/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:30:55 by oroy              #+#    #+#             */
/*   Updated: 2024/07/14 00:09:24 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler(void)
{
	_mimeTypes[".txt"] = "text/plain";
	_mimeTypes[".css"] = "text/css";
	_mimeTypes[".htm"] = "text/html";
	_mimeTypes[".html"] = "text/html";
	_mimeTypes[".js"] = "text/javascript";
	_mimeTypes[".apng"] = "image/apng";
	_mimeTypes[".avif"] = "image/avif";
	_mimeTypes[".gif"] = "image/gif";
	_mimeTypes[".jpg"] = "image/jpeg";
	_mimeTypes[".jpeg"] = "image/jpeg";
	_mimeTypes[".png"] = "image/png";
	_mimeTypes[".svg"] = "image/svg+xml";
	_mimeTypes[".webp"] = "image/webp";
	// 
	_statusCodeList[100] = "Continue";
	_statusCodeList[200] = "OK";
	_statusCodeList[201] = "Created";
	_statusCodeList[204] = "No Content";
	_statusCodeList[302] = "Found";
	_statusCodeList[400] = "Bad Request";
	_statusCodeList[403] = "Forbidden";
	_statusCodeList[404] = "Not Found";
	_statusCodeList[405] = "Method Not Allowed";
	_statusCodeList[500] = "Internal Server error";
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
	std::string	content = "<h1>404 Not Found</h1>";
	int			statusCode = 404;

	request.print_request();

	_htmlFile = _parseTarget(request.target());
	if (!request.isValid())
	{
		statusCode = 400;
		// _htmlFile = config.getErrorPage();	// 40x.html
		content = "<h1>Invalid Request</h1>";
	}
	else if (_htmlFile.find("../") != std::string::npos)
	{
		statusCode = 403;
		// _htmlFile = config.getErrorPage();	// 40x.html
		content = "<h1>Forbidden</h1>";
	}
	else
	{
		if (_htmlFile == "/")
		{
			// _htmlFile = config.getRootFile();
			_htmlFile = "/index.html";
		}
		// std::ifstream	f(config.getRoot() + _htmlFile, std::ios::binary);
		std::ifstream	f("./data/www" + _htmlFile, std::ios::binary);

		if (f.good())
		{
			std::string	str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
			content = str;
			statusCode = 200;
		}
		f.close();
	}

	std::ostringstream	oss;

	oss << request.version() << " " << statusCode << " " << _statusCodeList.at(statusCode) << "\r\n";
	oss << "Cache-Control: no-cache, private" << "\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	oss << "Content-Type: " << _getContentType(_htmlFile) << "\r\n";
	oss << "\r\n";
	oss << content;

	_response = oss.str();
	return _response;
}

std::string const	HttpHandler::_getContentType(std::string const &path)
{
	size_t		dotPos = path.rfind('.');
	std::string	ext;

	if (dotPos != std::string::npos)
	{
		ext = path.substr(dotPos);
		std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(ext);
		if (it != _mimeTypes.end())
		{
			return it->second;
		}
	}
	return ("text/html");
}

std::string	HttpHandler::_parseTarget(std::string const &target)
{
	size_t	queryPos = target.find('?');

	if (queryPos != std::string::npos)
	{
		return (target.substr(0, queryPos));
	}
	return (target);
}

// void	HttpHandler::_parseTarget(void)
// {
// 	std::string	needle = "../";
// 	size_t		found = 0;

// 	while ((found = _htmlFile.find(needle, found)) != std::string::npos)
// 	{
// 		_htmlFile.erase(found, needle.size());
// 	}
// }

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
	
// 	i = _htmlFile.find('/');
// 	i = _htmlFile.find('/', i);

// 	return (&_htmlFile[i + 1]);
// }
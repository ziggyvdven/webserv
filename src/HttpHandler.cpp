/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmehour <kmehour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:30:55 by oroy              #+#    #+#             */
/*   Updated: 2024/07/22 17:58:55 by kmehour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HttpHandler.hpp"
#include "../includes/CgiHandler.hpp"

HttpHandler::HttpHandler(WebServer const &webServer) : _webServer(webServer)
{
	(void) _webServer;
	
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

std::string const &	HttpHandler::buildResponse(HttpRequest const &request)
{
	std::string	content = "<h1>404 Not Found</h1>";
	int			statusCode = 404;

	_htmlFile = _parseTarget(request.target());
	if (!request.isValid() || _htmlFile.find("/../") != std::string::npos)
	{
		statusCode = 400;
		// _htmlFile = config.getErrorPage();	// 40x.html
		content = "<h1>Bad Request</h1>";
	}
	else
	{
		CgiHandler cgi_handler(request);
		if (cgi_handler.isCgiScript(request.target()))
		{
			content = cgi_handler.execCgiScript();
			statusCode = 200;
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
	}

	std::ostringstream	oss;

	oss << request.version() << " " << statusCode << " " << _statusCodeList.at(statusCode) << "\r\n";
	oss << "Cache-Control: no-cache, private" << "\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	// oss << "Content-Type: " << _getContentType() << "\r\n";
	// oss << "\r\n";
	oss << content;
	// oss << "<h3>[Server] Done</h3>";

	_response = oss.str();
	return (_response);
}

std::string const	HttpHandler::_getContentType(void)
{
	std::string	ext = _getExtension();

	if (!ext.empty())
	{
		std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(ext);
		if (it != _mimeTypes.end())
		{
			return it->second;
		}
	}
	return ("text/html");
}

std::string const	HttpHandler::_getExtension(void)
{
	size_t	dotPos = _htmlFile.rfind('.');

	if (dotPos != std::string::npos)
	{
		return (_htmlFile.substr(dotPos));
	}
	return ("");
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

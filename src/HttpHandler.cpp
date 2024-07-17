/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:30:55 by oroy              #+#    #+#             */
/*   Updated: 2024/07/16 20:37:58 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler(WebServer const &webServer) : _webServer(webServer), _htmlRoot("./data/www")
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
	//
	_cgiScripts["/upload.py"] = "/upload.html";
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

	_htmlFile = request.target();
	// _htmlFile = _parseTarget(request.target());
	if (!request.isValid() || _htmlFile.find("/../") != std::string::npos)
	{
		statusCode = 400;
		// _htmlFile = config.getErrorPage();	// 40x.html
		content = "<h1>Bad Request</h1>";
	}
	else
	{
		if (_htmlFile == "/")
		{
			// _htmlFile = config.getRootFile();
			_htmlFile = "/index.html";
		}
		else if (_isCGIScript(_htmlFile))
		{
			if (_execCGIScript(request))
			{
				_htmlFile = _cgiScripts.at(_htmlFile);
				std::cout << _htmlFile << std::endl;
			}
			else
			{
				content = "<h1>500 Internal Server Error</h1>";
				statusCode = 500;
			}
		}
		
		if (statusCode != 500)
		{
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
	oss << "Content-Type: " << _getContentType() << "\r\n";
	oss << "\r\n";
	oss << content;

	_response = oss.str();
	return (_response);
}

bool	HttpHandler::_isCGIScript(std::string const &target)
{
	std::string const	cgi_bin = "/cgi-bin/";
	size_t				it = target.find(cgi_bin);
	
	_scriptName = "SCRIPT_NAME=";
	_pathInfo = "PATH_INFO=";
	_queryString = "QUERY_STRING=";

	if (it == 0)
	{
		it = target.find_first_of("/?", cgi_bin.size());
		std::string const	script_name = target.substr(0, it);
		std::string const	script_path = _htmlRoot + script_name;
		if (access(script_path.c_str(), X_OK) == 0)
		{
			_scriptName += script_name;
			if (target[it] == '/')
			{
				size_t	it_query = target.find_first_of('?', it);
				_pathInfo += target.substr(it, it_query - it);
				it = it_query;
			}
			if (target[it] == '?')
			{
				_queryString += target.substr(it + 1);
			}
			std::cout << _scriptName << std::endl;
			std::cout << _pathInfo << std::endl;
			std::cout << _queryString << std::endl;
			return (true);
		}
	}
	return (false);
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

// std::string	HttpHandler::_parseTarget(std::string const &target)
// {
// 	size_t	queryPos = target.find('?');

// 	if (queryPos != std::string::npos)
// 	{
// 		return (target.substr(0, queryPos));
// 	}
// 	return (target);
// }

// void	HttpHandler::_parseTarget(void)
// {
// 	std::string	needle = "../";
// 	size_t		found = 0;

// 	while ((found = _htmlFile.find(needle, found)) != std::string::npos)
// 	{
// 		_htmlFile.erase(found, needle.size());
// 	}
// }

bool	HttpHandler::_execCGIScript(HttpRequest const &request) const
{
	std::string					script_path = "/Users/oroy/Documents/cursus42/webserv/cgi-bin" + _htmlFile;
	const char *				language_path = "/usr/bin/python";
	std::vector<char const *>	argv;
	std::vector<char const *>	envp;
	pid_t						process_id;

	std::string	const			version = "HTTP_VERSION=" + request.version();
	std::string	const			method = "METHOD=" + request.method();
	std::string const			filename = "FILENAME=/Users/oroy/Documents/cursus42/webserv/upload/test.txt";
	std::string	const			content = "CONTENT=This is a test";

	int							wstatus;

	process_id = fork();
	if (process_id < 0)
		perror ("fork() failed");
	else if (process_id == 0)
	{
		_webServer.cleanUpSockets();

		argv.push_back(language_path);
		argv.push_back(script_path.c_str());
		argv.push_back(NULL);

		envp.push_back(version.c_str());
		envp.push_back(method.c_str());
		envp.push_back(filename.c_str());
		envp.push_back(content.c_str());
		envp.push_back(NULL);

		execve (language_path, const_cast<char * const *>(argv.data()), const_cast<char * const *>(envp.data()));
		exit (EXIT_FAILURE);
	}
	waitpid (process_id, &wstatus, 0);
	if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
	{
		return (false);
	}
	return (true);
}

/*	Utils	***************************************************************** */

// char const	*HttpHandler::_getScriptName(void) const
// {
// 	size_t	i;
	
// 	i = _htmlFile.find('/');
// 	i = _htmlFile.find('/', i);

// 	return (&_htmlFile[i + 1]);
// }
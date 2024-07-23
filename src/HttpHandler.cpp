/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:30:55 by oroy              #+#    #+#             */
/*   Updated: 2024/07/22 20:15:00 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler(WebServer const &webServer, Config& conf) : _webServer(webServer), _config(conf), _baseDir("./data")
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
	_defaultPages[100] = "/default/10x.html";
	_defaultPages[200] = "/default/20x.html";
	_defaultPages[201] = "/default/20x.html";
	_defaultPages[204] = "/default/20x.html";
	_defaultPages[302] = "/default/30x.html";
	_defaultPages[400] = "/default/40x.html";
	_defaultPages[403] = "/default/40x.html";
	_defaultPages[404] = "/default/40x.html";
	_defaultPages[405] = "/default/40x.html";
	_defaultPages[500] = "/default/50x.html";
	// 
	_reasonPhrase[100] = "Continue";
	_reasonPhrase[200] = "OK";
	_reasonPhrase[201] = "Created";
	_reasonPhrase[204] = "No Content";
	_reasonPhrase[302] = "Found";
	_reasonPhrase[400] = "Bad Request";
	_reasonPhrase[403] = "Forbidden";
	_reasonPhrase[404] = "Not Found";
	_reasonPhrase[405] = "Method Not Allowed";
	_reasonPhrase[500] = "Internal Server error";
	// 
	// _method["GET"] = methodGET();
	// _method["POST"] = methodPOST();
	// _method["DELETE"] = methodDELETE();
	// 
	_headers["Accept-Ranges"] = "bytes";
	_headers["Allow"] = "GET, POST, DELETE";
	_headers["Cache-Control"] = "no-cache, private";
	_headers["Connection"] = "keep-alive";
	_headers["Content-Length"] = "0";
	_headers["Content-Type"] = "text/plain";
	_headers["Date"] = "";
	_headers["Location"] = "";
	_headers["Server"] = "";
	//
	_env["CONTENT_LENGTH"] = "";
	_env["PATH_INFO"] = "";
	_env["QUERY_STRING"] = "";
	_env["SCRIPT_NAME"] = "";
}

HttpHandler::~HttpHandler()
{

}

/*	Functions	************************************************************* */

std::string const	&HttpHandler::buildResponse(HttpRequest const &request)
{
	ConfigServer	config = _config.getServerConfig(request.getHeader("host"), request.target());

	_autoindex = false;
	_request = request;
	_htmlFile = _parseTarget(request.target());
	_path = _createPath(config);

	if (config.getRedirect().first != 0)
	{
		_statusCode = config.getRedirect().first;
		if (_statusCode >= 300 && _statusCode <= 399)
		{
			_headers["Location"] = config.getRedirect().second;
		}
		else
		{
			_content = config.getRedirect().second;
		}
	}
	else if (!config.getMethod(_request.method()))
	{
		_content = _getPage(config, 405);
		_statusCode = 405;
	}
	else
	{
		if (_request.method() == "GET")
			_get(config);
		else if (_request.method() == "POST")
			_post(config);
		else if (_request.method() == "DELETE")
			_delete(config);
	}

	// Open the file to put in the response body
	std::ifstream	file("/data" + config.getRoot() + _content, std::ios::binary);
	if (file.is_open())
	{
		_content = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	}
	else
	{
		_content = "<h1>500 Internal Server Error</h1>";
	}
	file.close();
	
	// Create the response
	std::ostringstream	response;

	response << _request.version() << " " << statusCode << " " << _statusCodeList.at(statusCode) << "\r\n";
	response << "Allow: " << _setAllow(config) << "\r\n";
	response << "Cache-Control: " << _getHeaderFieldValue("Cache-Control") << "\r\n";
	response << "Content-Length: " << _content.size() << "\r\n";
	response << "Content-Type: " << _getContentType() << "\r\n";
	response << "\r\n";
	response << _content;

	return (response);
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

std::string const HttpHandler::_createPath(ConfigServer& config)
{
	std::string	path;

	if (!_htmlFile.empty() && (_htmlFile == config.getTarget() || _htmlFile == config.getTarget() + "/"))
	{
		_htmlFile = "/" + config.getIndex();
	}
	if (!config.getTarget().empty())
	{
		size_t pos = _htmlFile.find(config.getTarget());
		if (pos != std::string::npos)
			_htmlFile.erase(pos, config.getTarget().length());
		path = _baseDir + config.getRoot() + _htmlFile;
	}
	else
	{
		path = _baseDir + config.getRoot() + _htmlFile;
	}
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
	{
		path = _baseDir + config.getRoot();
		if (config.getAutoIndex() == true)
			_autoindex = true;
	}
	close(fd);
	
	return path;
}

std::string	HttpHandler::_getPage(ConfigServer const &config, short const & errorcode) const
{
	std::string	htmlFile = config.getErrorPage(errorcode);

	if (htmlFile.empty())
	{
		return (_defaultPages[errorCode]);
	}
	return (htmlFile);
}

void	HttpHandler::_get(ConfigServer const &config) const
{
	try
	{
		if (_pathIsDirectory(config))
		{
			if (_autoIndex)
			{
				_content = _autoIndexGenerator(_path, _request.target(), config);
				_statusCode = 200;
				// goto response;
			}
			else
			{
				_content = _getPage(config, 403);
				_statusCode = 403;
			}
		}
		else if (_isCGIScript(_request.target()))
		{
			_execCGIScript(_request);
		}
		else
		{
			_openFile(config);
		}	
	}
	catch (std::exception const &e)
	{
		std::cerr << "Something wrong with _get() function" << std::endl;
	}
}

void	HttpHandler::_post(ConfigServer const &config) const
{
	try
	{
		if (_pathIsDirectory(config))
		{
			if (_autoIndex)
			{
				_content = _autoIndexGenerator(_path, _request.target(), config);
				_statusCode = 200;
			}
			else
			{
				_content = _getPage(config, 403);
				_statusCode = 403;
			}
		}
		else if (_isCGIScript(_request.target()))
		{
			_execCGIScript(_request);
		}
		else
		{
			_openFile(config);
		}
	}
	catch (std::exception const &e)
	{
		std::cerr << "Something wrong with _post() function" << std::endl;
	}
}

void	HttpHandler::_delete(ConfigServer const &config) const
{
	int result;

	if (access(file_path.c_str(), F_OK) == 0)
	{
		if (result != 0)
		{
			content = "<h1>Permission denied</h1>";
			statusCode = 403;
		}
		else
		{
			content = "<h1>File successfully deleted</h1>";
			statusCode = 200;
		}
	}
}

bool	HttpHandler::_pathIsDirectory(ConfigServer const &config) const
{
	struct stat s;

	if (stat(_path, &s) == 0)
	{
		if(s.st_mode & S_IFDIR)
		{
			return (true);
		}
	}
	else
	{
		_content = _getPage(config, 500);
		_statusCode = 500;
		throw std::exception();
	}
	return (false);
}

void	HttpHandler::_openFile(ConfigServer const &config) const
{
	std::ifstream	file("/data" + config.getRoot() + content, std::ios::binary);

	if (file.is_open())
	{
		_content = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	}
	else
	{
		_content = "<h1>500 Internal Server Error</h1>";
	}
	file.close();
}

std::string const	&HttpHandler::_getHeaderFieldValue(std::string const &field) const
{
	std::string	value = _request.getHeader(field);

	if (value.empty())
		return (_headers.at(field));
	return (value);
}

std::string const	&HttpHandler::_setAllow(ConfigServer const &config)
{
	std::string	methods;

	if (config.getMethod("GET"))
	{
		methods += "GET";
	}
	if (config.getMethod("POST"))
	{
		if (!methods.empty())
			methods += ", ";
		methods += "POST";
	}
	if (config.getMethod("DELETE"))
	{
		if (!methods.empty())
			methods += ", ";
		methods += "DELETE";
	}
	return (methods);
}

// if (!request.isValid() || _htmlFile.find("/../") != std::string::npos)
// {
// 	_statusCode = 400;
// 	_content = _getPage(config, 400);
// }

std::string const &	HttpHandler::buildResponse(HttpRequest const &request)
{
	std::string const	allowedMethods = "GET_POST_DELETE";
	std::string			content = "<h1>404 Not Found</h1>";
	int					statusCode = 404;

	request.print_request();

	_htmlFile = _parseTarget(request.target());
	ConfigServer settings = _config.getServerConfig(request.getHeader("host"), request.target());
	cout << settings << endl;
	
	if (!request.isValid() || _htmlFile.find("/../") != std::string::npos)
	{
		statusCode = 400;
		_htmlFile = settings.getErrorPage(400);
		if (_htmlFile.empty())
			_htmlFile = "/default_errors/40x.html";
	}
	else
	{
		if (_isCGIScript(request.target()))
		{
			_execCGIScript(request);
			content = "<h1>CGI Script Executed</h1>";
			statusCode = 200;
		}
		else
		{
			if (allowedMethods.find(request.method()) != std::string::npos)
			{
				if (_htmlFile == "/")
				{
					// _htmlFile = config.getRootFile();
					_htmlFile = "/index.html";
				}
				std::string const	file_path = _htmlRoot + _htmlFile;
				if (request.method() == "GET" || request.method() == "POST")
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
				else if (request.method() == "DELETE" && access(file_path.c_str(), F_OK) == 0)
				{
					int result = std::remove(file_path.c_str());
					if (result != 0)
					{
						content = "<h1>Permission denied</h1>";
						statusCode = 403;
					}
					else
					{
						content = "<h1>File successfully deleted</h1>";
						statusCode = 200;
					}					
				}
			}
			else
			{
				content = "<h1>Method Not Allowed</h1>";
				statusCode = 405;
			}
		}
	}
	
// 	std::ifstream	f("./data/www" + _htmlFile, std::ios::binary);

// 	if (f.good())
// 	{
// 		std::string	str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
// 		content = str;
// 		statusCode = 200;
// 	}
// 	f.close();

	std::ostringstream	oss;

	oss << request.version() << " " << statusCode << " " << _statusCodeList.at(statusCode) << "\r\n";
	oss << "Cache-Control: no-cache, private" << "\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	oss << "Content-Type: " << _getContentType() << "\r\n";
	oss << "Location: " << _getContentType() << "\r\n";
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

bool	HttpHandler::_execCGIScript(HttpRequest const &request) const
{
	std::string const			script_path = _htmlRoot + "/cgi-bin/upload.py";
	std::vector<char const *>	argv;
	std::vector<char const *>	envp;
	pid_t						process_id;

	std::string	const			version = "HTTP_VERSION=" + request.version();
	std::string	const			method = "METHOD=" + request.method();
	std::string const			filename = "FILENAME=./data/www/upload/test.txt";
	std::string	const			content = "This is a test";

	std::string	const			length = "CONTENT_LENGTH=14";
	
	int							wstatus;
	int							pipe_fd[2];

	pipe(pipe_fd);

	(void) _webServer;

	process_id = fork();
	if (process_id < 0)
		perror ("fork() failed");
	else if (process_id == 0)
	{
		// _webServer.cleanUpSockets();

		dup2 (pipe_fd[0], STDIN_FILENO);
		dup2 (pipe_fd[1], STDOUT_FILENO);

		close (pipe_fd[0]);
		close (pipe_fd[1]);
		
		std::cout << content << std::endl;

		argv.push_back(script_path.c_str());
		argv.push_back(NULL);

		envp.push_back(version.c_str());
		envp.push_back(method.c_str());
		envp.push_back(filename.c_str());
		envp.push_back(length.c_str());
		envp.push_back(NULL);

		execve (script_path.c_str(), const_cast<char * const *>(argv.data()), const_cast<char * const *>(envp.data()));
		exit (EXIT_FAILURE);
	}
	waitpid (process_id, &wstatus, WNOHANG);
	if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
	{
		return (false);
	}
	// read (pipe_fd[0], buffer, 255);
	close (pipe_fd[0]);
	close (pipe_fd[1]);
	// std::cout << "[] " << std::endl;
	// std::cout << buffer << std::endl;
	// std::cout << "[] " << std::endl;
	return (true);
}

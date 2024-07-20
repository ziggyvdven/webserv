/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zvan-de- <zvan-de-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:30:55 by oroy              #+#    #+#             */
/*   Updated: 2024/07/20 18:33:37 by zvan-de-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler(WebServer const &webServer, Config& conf) : _webServer(webServer), _config(conf), _htmlRoot("./data/www"), _autoindex(false)
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

std::string const &	HttpHandler::buildResponse(HttpRequest const &request)
{
	std::string const	allowedMethods = "GET_POST_DELETE";
	std::string			content = "<h1>404 Not Found</h1>";
	int					statusCode = 404;

	request.print_request();

	_autoindex = false;
	_htmlFile = _parseTarget(request.target());
	ConfigServer config = _config.getServerConfig(request.getHeader("host"), request.target());
	std::string path = _createPath(config);
	cout << config << endl;
	if (!request.isValid() || _htmlFile.find("/../") != std::string::npos)
	{
		statusCode = 400;
		_htmlFile = config.getErrorPage(400);
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
				if (request.method() == "GET" || request.method() == "POST")
				{
					cout << path << endl;
					std::ifstream	f(path, std::ios::binary);
					if (_autoindex == true){
						content = _autoIndexGeneator(path, request.target(), config);
						statusCode = 200;
						goto response;
					}
					else if (f.good())
					{
						std::string	str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
						content = str;
						statusCode = 200;
					}
					f.close();
				}
				else if (request.method() == "DELETE" && access(path.c_str(), F_OK) == 0)
				{
					int result = std::remove(path.c_str());
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

	// std::ifstream	f("./data/www" + _htmlFile, std::ios::binary);
	response:
	std::ostringstream	oss;

	oss << request.version() << " " << statusCode << " " << _statusCodeList.at(statusCode) << "\r\n";
	oss << "Cache-Control: no-cache, private" << "\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	oss << "Content-Type: " << _getContentType() << "\r\n";
	oss << "Location: " << "\r\n";
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

std::string	HttpHandler::_parseTarget(std::string const &target)
{
	size_t	queryPos = target.find('?');

	if (queryPos != std::string::npos)
	{
		return (target.substr(0, queryPos));
	}
	return (target);
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
	waitpid (process_id, &wstatus, 0);
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

std::string const HttpHandler::_autoIndexGeneator(std::string & path, std::string const & target, ConfigServer& config){
	DIR *dir;
	struct dirent *dp;
	const char* cpath = path.c_str();
	if ((dir = opendir(cpath)) == NULL){
		cerr << "Cannot open directory" << endl;
		return "";
	}
	
	std::string content =\
	"<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
            <title>" + path + "</title>\n\
    </head>\n\
    <body>\n\
    <h1>Index of " + target + "</h1>\n\
	<hr>\n\
    <p>\n";
	
	dp = readdir (dir);
	// cout << path << endl;
	while((dp = readdir (dir)) != NULL){
		// cout << dp->d_name << endl;
		content += "\t\t<p><a href=\"http://" + config.getHost() + ":" + to_string(config.getPort()) + target;
		if (target.back() != '/')
			content += "/";
		content += std::string(dp->d_name) + "\">" + dp->d_name + "</a></p>\n";
	}
	content +="\
	<hr>\n\
	</p>\n\
    </body>\n\
    </html>\n";
	
	return content;
}

std::string const HttpHandler::_createPath(ConfigServer& config){
	
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
		path = "./data" + config.getRoot() + _htmlFile;
	}
	else
	{
		path = "./data" + config.getRoot() + _htmlFile;
	}
	int fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
	{
		path = "./data" + config.getRoot();
		if (config.getAutoIndex() == true)
			_autoindex = true;
	}
	close(fd);
	
	return path;
}
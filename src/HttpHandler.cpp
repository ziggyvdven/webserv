/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:30:55 by oroy              #+#    #+#             */
/*   Updated: 2024/08/07 20:51:33 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HttpHandler.hpp"
#include "../includes/CgiHandler.hpp"

HttpHandler::HttpHandler(WebServer const &webServer, Config &conf) : _webServer(webServer), _conf(conf), _baseDir("./data"), _content("")
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
	_reasonPhrase[301] = "Moved Permanently";
	_reasonPhrase[302] = "Found";
	_reasonPhrase[303] = "See Other";
	_reasonPhrase[304] = "Not Modified";
	_reasonPhrase[307] = "Temporary Redirect";
	_reasonPhrase[308] = "Permanent Redirect";
	_reasonPhrase[400] = "Bad Request";
	_reasonPhrase[403] = "Forbidden";
	_reasonPhrase[404] = "Not Found";
	_reasonPhrase[405] = "Method Not Allowed";
	_reasonPhrase[413] = "Request Entity Too Large";
	_reasonPhrase[500] = "Internal Server error";
}

HttpHandler::~HttpHandler()
{

}

/*	Functions	************************************************************* */

void	HttpHandler::_setRequestParameters(HttpRequest const &request)
{
	_contentType = "text/html";
	_autoIndex = false;
	_contentType = "text/html";
	_getContentFromFile = false;
	_htmlFile = _parseTarget(request.target());
	_path = _createPath();
	_statusCode = 200;
	_autoIndex = false;
}

void	HttpHandler::buildResponse(HttpRequest const &request, HttpResponse &response)
{
	try {
		ConfigServer config = _conf.getServerConfig(request.getHeader("host"), request.target());
		_config = &config;
		
		_conf.printMsg(B, "Server[%s]: request recieved [Method[%s] Target[%s] Version[%s]]", _config->getServerName().c_str(), request.method().c_str(), request.target().c_str(), request.version().c_str());
		_setRequestParameters(request);
		bool	redirect = _check_redirect();
		if (!_check_40x_error( request ) && !redirect)
		{
			_contentType = _getContentType();
			if (request.method() == "GET")
				_get(request);
			else if (request.method() == "POST")
				_post(request);
			else if (request.method() == "DELETE")
				_delete();
		}
		if (!redirect){
			while (_getContentFromFile)
				_openFile();
		}
		_server_msg();
		_populateResponse(request, response);
	}
	catch (exception &e){
		cerr << "webserv: " << R << "ERROR" << END << "[" << e.what() << "]" << endl;
	}
}

bool	HttpHandler::_check_redirect()
{
	if (_config->getRedirect().first != 0)
	{
		_statusCode = _config->getRedirect().first;
		if ((_statusCode >= 301 && _statusCode <= 304) || _statusCode == 307 || _statusCode == 308)
		{
			// _headers["Location"] = _config->getRedirect().second;
			response.setHeader("Location", _config->getRedirect().second);
			return true;
		}
		else
		{
			if (_config->getRedirect().second.empty()){
				_content = _getPage(_statusCode);
			}
			else{
				_content = _config->getRedirect().second;
				return true;
			}
		}
	}
	return false;
}

bool	HttpHandler::_check_40x_error( HttpRequest const &request )
{
	if (_config->getClientMaxBodySize() && request.body().size() > _config->getClientMaxBodySize())
	{
		_content = _getPage(413);
		_statusCode = 413;
		return true;
	}
	else if (!_config->getMethod(request.method()))
	{
		_content = _getPage(405);
		_statusCode = 405;
		return true;
	}
	else if (access(_path.c_str(), F_OK) != 0)
	{
		_content = _getPage(404);
		_statusCode = 404;
		return true;
	}
	return false;
}

bool	HttpHandler::_isDirectory(const char *cpath){
	DIR *dir = opendir(cpath);
	if (dir == NULL){
		return false;
	}
	closedir(dir);
	return true;
}

void	HttpHandler::_server_msg(){
		
	if (_autoIndex && _isDirectory(_path.c_str()))
		_conf.printMsg(B, "Server[%s]: sending response [%s directory listing][%d]", _config->getServerName().c_str() , _path.c_str(),_statusCode);
	else if (_statusCode == 200)
		_conf.printMsg(B, "Server[%s]: sending response [%s][%d]", _config->getServerName().c_str(), _path.c_str(), _statusCode);
	else if (_config->getErrorPage(_statusCode) != "")
		_conf.printMsg(B, "Server[%s]: sending response [%s][%d]", _config->getServerName().c_str(), _path.c_str(), _statusCode);
	else
		_conf.printMsg(B, "Server[%s]: sending response [Default page][%d]", _config->getServerName().c_str(), _statusCode);
		
}

void	HttpHandler::_populateResponse(HttpRequest const &request, HttpResponse &response)
{
	response.setContent(_content);
	response.setHeader("Allow", _setAllow());
	response.setHeader("Content-Length", std::to_string(_content.size()));
	response.setHeader("Content-Type", _contentType);
	response.setHeader("Date", _conf.getCurrTime());
	response.setStatusCode(_statusCode);
	response.setVersion(request.version());
}

// std::string const	HttpHandler::_response(HttpRequest const &request)
// {
// 	std::ostringstream	response;

// 	response << request.version() << " " << _statusCode << " " << _reasonPhrase[_statusCode] << "\r\n";
// 	response << "Allow: " << _setAllow() << "\r\n";
// 	response << "Cache-Control: " << _getHeaderFieldValue(request, "Cache-Control") << "\r\n";
// 	response << "Content-Length: " << _content.size() << "\r\n";
// 	response << "Content-Type: " << _contentType << "\r\n";
// 	response << "Location: " << _getHeaderFieldValue(request, "Location") << "\r\n";
// 	response << "\r\n";
// 	response << _content;

// 	return (response.str());
// }

std::string	HttpHandler::_setDefaultContent(short const & errorCode)
{
	std::ostringstream	content;

	content << "<!DOCTYPE html>" << "\n";
	content << "<html>" << "\n";
	content << "<style>" << "\n";
	content << "h1 {text-align: center;}" << "\n";
	content << "</style>" << "\n";
	content << "<body>" << "\n";
	content << "<h1>" << errorCode << " " << _reasonPhrase[errorCode] << "</h1>" << "\n";
	content << "<hr>" << "\n";
	content << "</body>" << "\n";
	content << "</html>" << "\n";

	return (content.str());
}

std::string	HttpHandler::_getPage(short const & errorCode)
{
	std::string	file = _config->getErrorPage(errorCode);
	std::string path = _baseDir + _config->getRoot() + "/" + file;

	if (file.empty() || access(path.c_str(), F_OK) != 0 || _config->getRedirect().first != 0)
	{
		_getContentFromFile = false;
		return (_setDefaultContent(errorCode));
	}
	_getContentFromFile = true;
	_path = path;
	return (file);
}

void	HttpHandler::_openFile()
{
	std::ifstream	file(_path, std::ios::binary);

	if (file.is_open())
	{
		std::string	str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		_content = str;
		_getContentFromFile = false;
	}
	else
	{
		_content = _getPage(500);
		_statusCode = 500;
	}
	file.close();
}

bool	HttpHandler::_pathIsDirectory(std::string path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
	{
		if(s.st_mode & S_IFDIR)
		{
			return (true);
		}
	}
	else
	{
		_content = _getPage(500);
		_statusCode = 500;
		throw std::exception();
	}
	return (false);
}

/*	Target Parsing	********************************************************* */

std::string	HttpHandler::_createPath( void )
{
	std::string	path;
	std::string temp;

	path = _baseDir + _config->getRoot() + _htmlFile;
	if (!_config->getTarget().empty() && _config->getTarget() != "/")	// Added _config->getTarget() != "/" here
	{
		size_t pos = path.find(_config->getTarget());
		if (pos != std::string::npos)
			path.erase(pos, _config->getTarget().length());
	}
	if (_isDirectory(path.c_str())){
		if (!_htmlFile.empty() && _htmlFile.back() == '/')
			temp = path + _config->getIndex();
		else
			temp = path + "/" + _config->getIndex();
	}
	if (access(temp.c_str(), F_OK) == -1)
	{
		if (_config->getAutoIndex())
			_autoIndex = true;
	}
	else
		path = temp;
	return path;
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

/*	Methods		************************************************************* */

void	HttpHandler::_get( HttpRequest const &request)
{
	try
	{
		CgiHandler cgi_handler(request, _config);
		
		if ((_isDirectory(_path.c_str())))
		{
			if (_autoIndex)
				_content = _autoIndexGenerator(_path, request.target());
			else
			{
				_content = _getPage(403);
				_statusCode = 403;
			}
		}
		else if (cgi_handler.isCgiScript(request.target()))
		{
			_content = cgi_handler.execCgiScript();
		}
		else if (access(_path.c_str(), F_OK) == 0)
			_getContentFromFile = true;
	}
	catch (std::exception const &e)
	{
		std::cerr << "Something wrong with _get() function" << std::endl;
	}
}

void	HttpHandler::_post(HttpRequest const &request)
{
	try
	{
		CgiHandler cgi_handler(request, _config);

		if (_pathIsDirectory(_path))
		{
			if (_autoIndex)
				_content = _autoIndexGenerator(_path, request.target());
			else
			{
				_content = _getPage(403);
				_statusCode = 403;
			}
		}
		else if (cgi_handler.isCgiScript(request.target()))
		{
			_content = cgi_handler.execCgiScript();
		}
		else
			_getContentFromFile = true;
	}
	catch (std::exception const &e)
	{
		std::cerr << "Something wrong with _post() function" << std::endl;
	}
}

void	HttpHandler::_delete( void )
{
	// No need to check for file existence.
	// We already know since we tried opening it in the createPath() function.
	if (std::remove(_path.c_str()) == 0)
	{
		_content = _getPage(200);
		_statusCode = 200;
	}
	else
	{
		_content = _getPage(403);
		_statusCode = 403;
	}
}

/*	Header Fields	********************************************************* */

std::string const	HttpHandler::_setAllow( )
{
	std::string	methods;

	if (_config->getMethod("GET"))
	{
		methods += "GET";
	}
	if (_config->getMethod("POST"))
	{
		if (!methods.empty())
			methods += ", ";
		methods += "POST";
	}
	if (_config->getMethod("DELETE"))
	{
		if (!methods.empty())
			methods += ", ";
		methods += "DELETE";
	}
	return (methods);
}

std::string const	HttpHandler::_getContentType(void)
{
	std::string	ext = _getExtension();

	if (!ext.empty())
	{
		for (std::map<std::string const, std::string>::const_iterator it = _mimeTypes.begin(); it != _mimeTypes.end(); ++it)
		{
			if (it->first == ext)
			{
				return it->second;
			}
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

/*	AutoIndex	************************************************************* */

std::string const HttpHandler::_autoIndexGenerator(std::string & path, std::string const & target){
	DIR *dir;
	struct dirent *dp;
	const char* cpath = path.c_str();
	if ((dir = opendir(cpath)) == NULL){
		cerr << "Cannot open directory" << endl;
		return "";
	}
	std::string cut_path = path.substr(7);
	if (cut_path.back() != '/')
		cut_path += '/';
	std::string content =\
	"<!DOCTYPE html>\n\
    <html lang=\"en\">\n\
    <head>\n\
            	<meta charset=\"utf-8\">\n\
				<title>Welcome to our webserv</title>\n\
				<link rel=\"stylesheet\" href=\"styles/styles.css\">\n\
    </head>\n\
    <body>\n\
	<div id=\"main-container\">\n\
		<nav id=\"menu\">\n\
			<ul>\n\
				<li><a href=\".\">Home</a></li>\n\
				<li><a href=\"#\">CGI</a>\n\
					<ul>\n\
						<li><a href=\"upload.html\">Upload</a></li>\n\
						<li><a href=\"time.html\">Time</a></li>\n\
					</ul>\n\
				</li>\n\
				<li><a href=\"about.html\">About</a></li>\n\
			</ul>\n\
		</nav>\n\
    <h1>Index of " + cut_path + "</h1>\n\
	<hr>\n\
    <p>\n";
	
	dp = readdir (dir);
	while((dp = readdir (dir)) != NULL){
		// cout << dp->d_name << endl;
		content += "\t\t<p><a href=\"http://" + _config->getHost() + ":" + to_string(_config->getPort()) + target;
		if (target.back() != '/')
			content += "/";
		content += std::string(dp->d_name) + "\">" + dp->d_name + "</a></p>\n";
	}
	content +="\
	<hr>\n\
	</p>\n\
			<footer>\n\
			<p>\n\
				<img src=\"images/triforce.png\" width=\"16\" height=\"13\">\n\
				&nbsp;&nbsp;All Hail Ganondorf&nbsp;&nbsp;\n\
				<img src=\"images/triforce.png\" width=\"16\" height=\"13\">\n\
			</p>\n\
		</footer>\n\
	</div>\n\
    </body>\n\
    </html>\n";
	closedir(dir);
	return content;
}

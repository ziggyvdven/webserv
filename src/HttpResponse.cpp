#include "../includes/HttpResponse.hpp"
#include "../includes/utils.hpp"

HttpResponse::HttpResponse(void)
{
	_content = "";
	_statusCode = 200;
	_version = "HTTP/1.1";

	_headers["allow"] = "GET";
	_headers["cache-control"] = "no-cache, private";
	_headers["connection"] = "keep-alive";
	_headers["content-length"] = "0";
	_headers["content-type"] = "text/html";
	_headers["date"] = "";
	_headers["location"] = "";

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

HttpResponse::HttpResponse(HttpResponse const &src)
{
	*this = src;
}

HttpResponse	&HttpResponse::operator=(HttpResponse const &rhs)
{
	if (this != &rhs)
	{
		_content = rhs._content;
		_headers = rhs._headers;
		_reasonPhrase = rhs._reasonPhrase;
		_statusCode = rhs._statusCode;
		_version = rhs._version;
	}
	return (*this);
}

HttpResponse::~HttpResponse() {}

/*	Getters	***************************************************************** */

std::string const	HttpResponse::getResponse(void) const
{
	std::ostringstream	response;

	response << getStatusLine() << "\r\n";
	response << getHeaders() << "\r\n";
	response << getContent();

	return (response.str());
}

std::string const	HttpResponse::getStatusLine(void) const
{
	std::ostringstream	statusLine;

	statusLine << _version;
	statusLine << " ";
	statusLine << _statusCode;
	statusLine << " ";
	statusLine << _reasonPhrase.at(_statusCode);

	return (statusLine.str());
}

std::string const	HttpResponse::getHeaders(void) const
{
	std::ostringstream	headers;

	for (std::map<std::string, std::string>::const_iterator cit = _headers.begin(); cit != _headers.end(); ++cit)
	{
		headers << cit->first << ": " << cit->second << "\r\n";
	}
	return (headers.str());
}

std::string const	HttpResponse::getContent(void) const
{
	return (_content);
}

std::string const	HttpResponse::getContentType(std::string const &ext) const
{
	std::map<std::string const, std::string>::const_iterator	it;

	if (!ext.empty())
	{
		for (it = _mimeTypes.begin(); it != _mimeTypes.end(); ++it)
		{
			if (it->first == ext)
			{
				return it->second;
			}
		}
	}
	return ("text/html");
}

std::string const	HttpResponse::getDefaultContent(short const &statusCode) const
{
	std::ostringstream	content;

	content << "<!DOCTYPE html>" << "\n";
	content << "<html>" << "\n";
	content << "<style>" << "\n";
	content << "h1 {text-align: center;}" << "\n";
	content << "</style>" << "\n";
	content << "<body>" << "\n";
	content << "<h1>" << statusCode << " " << _reasonPhrase.at(statusCode) << "</h1>" << "\n";
	content << "<hr>" << "\n";
	content << "</body>" << "\n";
	content << "</html>" << "\n";

	return (content.str());
}

/*	Setters	***************************************************************** */

void	HttpResponse::setContent(std::string const &content)
{
	_content = content;
}

void	HttpResponse::setHeader(std::string const &key, std::string const &value)
{
	std::string	keyLowerCase = key;

	toLowerCase(keyLowerCase);
	_headers[keyLowerCase] = value;
}

void	HttpResponse::setStatusCode(short const &statusCode)
{
	_statusCode = statusCode;
}

void	HttpResponse::setVersion(std::string const &version)
{
	_version = version;
}

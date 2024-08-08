#include "../includes/HttpResponse.hpp"
#include "../includes/utils.hpp"

HttpResponse::HttpResponse(void)
{
	_content = "";
	_statusCode = 200;
	_version = "HTTP/1.1";

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

	_headers["allow"] = "GET";
	_headers["cache-control"] = "no-cache, private";
	_headers["connection"] = "keep-alive";
	_headers["content-length"] = "0";
	_headers["content-type"] = "text/html";
	_headers["date"] = "";
	_headers["location"] = "";
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

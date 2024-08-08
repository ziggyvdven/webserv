#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>

class HttpResponse
{
private:

	short										_statusCode;
	std::string									_content;
	std::string									_version;

	std::map<std::string, std::string>			_headers;
	std::map<std::string const, std::string>	_mimeTypes;
	std::map<short const, std::string>			_reasonPhrase;

public:

	HttpResponse(void);
	HttpResponse(HttpResponse const &src);
	HttpResponse &operator=(HttpResponse const &rhs);
	~HttpResponse();

	std::string const							getContent(void) const;
	std::string const							getContentType(std::string const &ext) const;
	std::string const							getDefaultContent(short const &statusCode) const;
	std::string const							getHeaders(void) const;
	std::string const							getResponse(void) const;
	std::string const							getStatusLine(void) const;

	void										setContent(std::string const &content);
	void										setHeader(std::string const &key, std::string const &value);
	void										setVersion(std::string const &version);
	void										setStatusCode(short const &statusCode);

};

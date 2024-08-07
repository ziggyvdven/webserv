/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:30:55 by oroy              #+#    #+#             */
/*   Updated: 2024/07/04 18:41:42 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HttpHandler.hpp"

HttpHandler::HttpHandler(void)
{

}

HttpHandler::~HttpHandler()
{

}

/*	Field List	************************************************************* */

std::string const	HttpHandler::_fieldList[FIELD_COUNT] = \
{
	"Host"
};

/*	Functions	************************************************************* */

std::string const	HttpHandler::handleRequest(std::string request)
{
	size_t	i = 0;
	size_t	f = 0;

	_request = request;

	f = _request.find(' ', i);
	_method = _request.substr(i, f - i);

	i = f + 1;
	f = _request.find(' ', i);
	_target = _request.substr(i, f - i);
	if (_target == "/")
		_target = "/index.html";

	i = f + 1;
	f = _request.find('\n', i);
	_version = _request.substr(i, f - i);

	// i = _parseHeaderFields(i, f);
	
	// if (_request[i])
	// 	_body = &_request[i];

	_buildResponse();
	return (_response);
}

/**
 * Don't forget to fix issue with not found header field in array
*/
size_t	HttpHandler::_parseHeaderFields(size_t i, size_t f)
{
	std::string	key;
	std::string	value;
	size_t 		count = 0;

	while (count < FIELD_COUNT)
	{
		i = _request.find('\n', i) + 1;
		f = _request.find(':', i);
		key = _request.substr(i, f - i);
		if (_findField(key) >= 0)
		{
			i = f + 1;
			i = _request.find_first_not_of(' ', i);
			f = _request.find('\n', i);
			value = _request.substr(i, f - i);
			_fields.insert(std::pair<std::string, std::string>(key, value));
			count++;
		}
	}
	while (i + 1 != f)	// Skip all other header fields
	{
		i = _request.find('\n', i) + 1;
		f = _request.find('\n', i);
	}
	return (f + 1);
}

size_t	HttpHandler::_findField(std::string const key) const
{
	for (size_t i = 0; i < FIELD_COUNT; ++i)
	{
		if (key == _fieldList[i])
			return (i);
	}
	return (-1);
}

void	HttpHandler::_buildResponse()
{
	std::ifstream		f("./data/www" + _target, std::ios::binary);
	std::ostringstream	oss;
	std::string			content = _request;
	int					errorCode = 404;

	if (f.good())
	{
		std::string	str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
		content = str;
		errorCode = 200;
	}
	f.close();

	oss << "HTTP/1.1 " << errorCode << " OK\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	// oss << "Content-Type: " << contentType << "\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";
	oss << "\r\n";
	oss << content;

	_response = oss.str();
}

/*	Utils	***************************************************************** */

void	HttpHandler::_printHeaderFields(void) const
{
	for (std::map<std::string, std::string>::const_iterator it = _fields.begin(); it != _fields.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << "\n";
	}
}

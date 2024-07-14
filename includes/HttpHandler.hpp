/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:09:00 by oroy              #+#    #+#             */
/*   Updated: 2024/07/14 00:08:50 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPHANDLER_HPP
# define HTTPHANDLER_HPP

# include <unistd.h>
# include <fstream>
# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>
# include "HttpRequest.hpp"

# define FIELD_COUNT	1

class HttpHandler
{
private:

	std::map<std::string, std::string>	_mimeTypes;
	std::map<int, std::string>			_statusCodeList;

	std::string							_response;

	std::map<std::string, std::string>	_fields;
	std::string							_body;

	std::string							_htmlFile;

	// void								_execCgiScript(void) const;
	// char const						*_getScriptName(void) const;

	std::string const					_getContentType(std::string const &path);
	std::string							_parseTarget(std::string const &target);

	
public:

	HttpHandler(void);
	~HttpHandler();

	// std::string const				getResponse(void) const;
	// std::string const				handleRequest(HttpRequest const &request);
	std::string const &					buildResponse(HttpRequest const &request);
};

#endif

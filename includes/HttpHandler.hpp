/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:09:00 by oroy              #+#    #+#             */
/*   Updated: 2024/07/13 19:48:17 by oroy             ###   ########.fr       */
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
#include "../includes/HttpRequest.hpp"

# define FIELD_COUNT	1

class HttpHandler
{
private:
	std::string							_response;

	std::map<std::string, std::string>	_fields;
	std::string							_body;

	// int									_statusCode;
	std::string							_target;

	// void								_execCgiScript(void) const;
	// char const						*_getScriptName(void) const;

	void								_parseTarget(void);

	
public:

	HttpHandler(void);
	~HttpHandler();

	// std::string const				getResponse(void) const;
	// std::string const				handleRequest(HttpRequest const &request);
	std::string const &					buildResponse(HttpRequest const &request);
};

#endif

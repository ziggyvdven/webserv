/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:09:00 by oroy              #+#    #+#             */
/*   Updated: 2024/07/05 16:06:18 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPHANDLER_HPP
# define HTTPHANDLER_HPP

# include <unistd.h>
# include <fstream>
# include <iostream>
# include <sstream>
# include <string>
# include <map>

# define FIELD_COUNT	1

class HttpHandler
{
private:

	std::string							_request;
	std::string							_response;

	std::string							_method;
	std::string							_target;
	std::string							_version;

	std::map<std::string, std::string>	_fields;
	std::string							_body;

	static std::string const			_fieldList[FIELD_COUNT];

	void								_execCgiScript(void) const;
	std::string							_getScriptName(void) const;

	size_t								_findField(std::string const key) const;
	void								_buildResponse();
	size_t								_parseHeaderFields(size_t i, size_t f);

	void								_printHeaderFields(void) const;

public:

	HttpHandler(void);
	~HttpHandler();

	// std::string const					getResponse(void) const;
	std::string const					handleRequest(std::string request);
};

#endif

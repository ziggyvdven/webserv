/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zvan-de- <zvan-de-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:09:00 by oroy              #+#    #+#             */
/*   Updated: 2024/07/20 17:04:35 by zvan-de-         ###   ########.fr       */
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
# include "WebServer.hpp"
# include "Config.hpp"
# include <sys/types.h>
# include <dirent.h>
# include <libgen.h>

class WebServer;

class HttpHandler
{
private:

	WebServer const &					_webServer;
	Config&								_config;
	std::string	const					_htmlRoot;

	std::string							_scriptName;
	std::string							_pathInfo;
	std::string							_queryString;

	std::string							_host;
	std::string							_port;
	

	std::map<std::string, std::string>	_mimeTypes;
	std::map<int, std::string>			_statusCodeList;

	std::string							_response;

	std::map<std::string, std::string>	_fields;
	std::string							_body;

	std::string							_htmlFile;

	bool								_autoindex;

	bool								_execCGIScript(HttpRequest const &request) const;
	bool								_isCGIScript(std::string const &target);
	// char const						*_getScriptName(void) const;

	std::string const					_getContentType(void);
	std::string const					_getExtension(void);
	std::string							_parseTarget(std::string const &target);
	std::string const					_autoIndexGeneator(std::string& path, std::string const & target, ConfigServer& config);
	std::string const 					_createPath( ConfigServer& config );

	
public:

	HttpHandler(WebServer const &webServer, Config & conf);
	~HttpHandler();

	// std::string const				getResponse(void) const;
	// std::string const				handleRequest(HttpRequest const &request);
	std::string const &					buildResponse(HttpRequest const &request);
};

#endif

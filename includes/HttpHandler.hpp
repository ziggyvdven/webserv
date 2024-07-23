/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:09:00 by oroy              #+#    #+#             */
/*   Updated: 2024/07/22 20:01:55 by oroy             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPHANDLER_HPP
# define HTTPHANDLER_HPP

# include <unistd.h>
# include <sys/stat.h>
# include <fstream>
# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <map>
# include "HttpRequest.hpp"
# include "WebServer.hpp"
# include "Config.hpp"
# include "ConfigServer.hpp"

class WebServer;

class HttpHandler
{
private:

	Config&								_config;
	WebServer const &					_webServer;
	std::string	const					_htmlRoot;

	// std::map<std::string const, void (HttpHandler::*)(Request &, RequestConfig &)>	_method;

	HttpRequest const &					_request;

	std::map<std::string const, std::string>	_env;
	std::map<std::string const, std::string>	_headers;
	std::map<std::string const, std::string>	_mimeTypes;
	std::map<int, std::string const>			_defaultPages;
	std::map<int, std::string const>			_reasonPhrase;

	bool								_autoIndex;
	std::string							_allowedMethods;
	std::string							_content;
	std::string							_statusCode;

	std::string							_baseDir;
	std::string							_htmlFile;
	std::string							_path;

	std::string							_host;
	std::string							_port;

	std::string							_queryString;
	std::string							_pathInfo;
	std::string							_scriptName;

	std::string							_response;

	std::map<std::string, std::string>	_fields;
	std::string							_body;


	std::string							_parseTarget(std::string const &target);
	bool								_pathIsDirectory(ConfigServer const &config) const;

	std::string const					_getHeaderFieldValue(std::string const &field) const;
	std::string const					_setAllow(ConfigServer const &config);				

	bool								_execCGIScript(HttpRequest const &request) const;
	bool								_isCGIScript(std::string const &target);

	std::string const					_getContentType(void);
	std::string const					_getExtension(void);

	std::string							_getPage(ConfigServer const &config, short const & errorcode) const;

	void								_get(HttpRequest const &request, ConfigServer const &config) const;
	void								_post(HttpRequest const &request, ConfigServer const &config) const;
	void								_delete(HttpRequest const &request, ConfigServer const &config) const;

	void								_openFile(ConfigServer const &config) const;			

	
public:

	HttpHandler(WebServer const &webServer, Config & conf);
	~HttpHandler();

	std::string const &						buildResponse(HttpRequest const &request);
};

#endif

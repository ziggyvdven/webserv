/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olivierroy <olivierroy@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:09:00 by oroy              #+#    #+#             */
/*   Updated: 2024/07/23 01:28:53 by olivierroy       ###   ########.fr       */
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
# include <dirent.h>
# include <map>
# include "HttpRequest.hpp"
# include "WebServer.hpp"
# include "Config.hpp"
# include "ConfigServer.hpp"

class WebServer;

class HttpHandler
{
private:

	// Parameters at contruction -- Start

	Config &							_conf;
	WebServer const &					_webServer;
	std::string const					_baseDir;

	std::map<std::string const, std::string>	_env;
	std::map<std::string const, std::string>	_headers;
	std::map<std::string const, std::string>	_mimeTypes;
	std::map<int, std::string const>			_defaultPages;
	std::map<int, std::string const>			_reasonPhrase;

	// std::map<std::string const, void (HttpHandler::*)(Request &, RequestConfig &)>	_method;

	// Parameters at contruction -- End

	ConfigServer &						_config;
	HttpRequest &						_request;

	bool								_autoIndex;
	std::string							_allowedMethods;
	std::string							_content;
	bool								_getContentFromFile;
	std::string							_statusCode;

	std::string							_htmlFile;
	std::string							_path;

	std::string							_queryString;
	std::string							_pathInfo;
	std::string							_scriptName;

	std::map<std::string, std::string>	_fields;
	std::string							_body;

	void								_setRequestParameters(request);
	std::string							_parseTarget(std::string const &target);
	std::string 						_createPath(void);
	std::string							_getPage(short const & errorcode) const;
	std::string							_setDefaultContent(short const & errorcode);
	bool								_pathIsDirectory(void) const;
	void								_openFile(void) const;	

	void								_get(HttpRequest const &request);
	void								_post(HttpRequest const &request);
	void								_delete(HttpRequest const &request);

	std::string const					_response(void) const;
	std::string const					_setAllow(void);
	std::string const					_getContentType(void);
	std::string const					_getExtension(void);
	std::string const					_getHeaderFieldValue(std::string const &field) const;

	bool								_execCGIScript(HttpRequest const &request) const;
	bool								_isCGIScript(std::string const &target);

public:

	HttpHandler(WebServer const &webServer, Config & conf);
	~HttpHandler();

	std::string const &					buildResponse(HttpRequest const &request);
};

#endif

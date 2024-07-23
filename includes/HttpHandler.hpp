/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:09:00 by oroy              #+#    #+#             */
/*   Updated: 2024/07/23 13:37:11 by oroy             ###   ########.fr       */
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

	WebServer const &					_webServer;
	Config &							_conf;
	std::string const					_baseDir;

	std::map<std::string const, std::string>	_env;
	std::map<std::string const, std::string>	_headers;
	std::map<std::string const, std::string>	_mimeTypes;
	std::map<int, std::string>					_defaultPages;
	std::map<int, std::string>					_reasonPhrase;

	// Parameters at contruction -- End

	bool								_autoIndex;
	std::string							_allowedMethods;
	std::string							_content;
	bool								_getContentFromFile;
	int									_statusCode;

	std::string							_htmlFile;
	std::string							_path;

	std::string							_queryString;
	std::string							_pathInfo;
	std::string							_scriptName;

	std::map<std::string, std::string>	_fields;
	std::string							_body;

	void								_setRequestParameters(ConfigServer const &config, HttpRequest const &request);
	std::string							_parseTarget(std::string const &target);
	std::string 						_createPath(ConfigServer const &config);
	std::string							_getPage(ConfigServer const &config, short const & errorCode);
	std::string							_setDefaultContent(short const & errorCode);
	bool								_pathIsDirectory(ConfigServer const &config);
	void								_openFile(ConfigServer const &config);	

	void								_get(ConfigServer const &config, HttpRequest const &request);
	void								_post(ConfigServer const &config, HttpRequest const &request);
	void								_delete(ConfigServer const &config);

	std::string const					_response(ConfigServer const &config, HttpRequest const &request);
	std::string const					_setAllow(ConfigServer const &config);
	std::string const					_getContentType(void);
	std::string const					_getExtension(void);
	std::string const					_getHeaderFieldValue(HttpRequest const &request, std::string const &field);

	std::string const					_autoIndexGenerator(std::string & path, std::string const & target, ConfigServer const &config);

	bool								_execCGIScript(HttpRequest const &request) const;
	bool								_isCGIScript(std::string const &target);

public:

	HttpHandler(WebServer const &webServer, Config &conf);
	~HttpHandler();

	std::string const					buildResponse(HttpRequest const &request);
};

#endif

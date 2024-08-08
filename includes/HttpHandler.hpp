/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oroy <oroy@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 12:09:00 by oroy              #+#    #+#             */
/*   Updated: 2024/08/07 20:52:51 by oroy             ###   ########.fr       */
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
# include "HttpResponse.hpp"
# include "WebServer.hpp"
# include "Config.hpp"
# include "ConfigServer.hpp"

class WebServer;

class HttpHandler
{
private:

	// Parameters at contruction -- Start

	WebServer const &					_webServer;
	ConfigServer*						_config;
	Config &							_conf;
	std::string const					_baseDir;

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
	std::string							_contentType;

	std::map<std::string, std::string>	_fields;
	std::string							_body;

	void								_setRequestParameters(HttpRequest const &request);
	std::string							_parseTarget(std::string const &target);
	std::string 						_createPath( void );
	std::string							_getPage(short const & errorCode);
	std::string							_setDefaultContent(short const & errorCode);
	bool								_pathIsDirectory(std::string path);
	void								_openFile();
	void								_server_msg( void );
	bool								_isDirectory(const char *path);
	bool								_check_40x_error( HttpRequest const &request );
	bool								_check_redirect();

	void								_get(HttpRequest const &request);
	void								_post(HttpRequest const &request);
	void								_delete( void );

	void								_populateResponse(HttpRequest const &request, HttpResponse &response);
	std::string const					_setAllow();
	std::string const					_getContentType(void);
	std::string const					_getExtension(void);
	std::string const					_getHeaderFieldValue(HttpRequest const &request, std::string const &field);

	std::string const					_autoIndexGenerator(std::string & path, std::string const & target);

	bool								_execCGIScript(HttpRequest const &request) const;
	bool								_isCGIScript(std::string const &target);
	// char const						*_getScriptName(void) const;

public:

	HttpHandler(WebServer const &webServer, Config &conf);
	~HttpHandler();

	ConfigServer*						getConfigServer( void ) { return _config; };
	std::string	const					getPath( void ) const { return _path; };
	void								buildResponse(HttpRequest const &request, HttpResponse &response);
};

#endif

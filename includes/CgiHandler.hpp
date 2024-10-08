#pragma once

#include "../includes/HttpRequest.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include "ConfigServer.hpp"


class CgiHandler
{
private:
	HttpRequest	const	&_request;

	std::string			_htmlRoot;
	bool				_is_valid;
	std::string			_scriptName;
	std::string			_scriptPath;
	std::string			_pathInfo;
	std::string			_queryString;
	std::string			_cgiResponse;
	ConfigServer*		_ConfigServer;
	
	std::vector<char const *>	_envp;

	void	_init();
	bool	_timeout_cgi(int process_id, int &wstatus, int timeout_sec);

public:
	CgiHandler(HttpRequest const &request, ConfigServer* config);

	bool	isCgiRequest() const; 
	bool	isCgiScript(std::string const &target);
	std::string	execCgiScript();
};

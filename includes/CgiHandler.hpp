#pragma once

#include "../includes/HttpRequest.hpp"
#include <iostream>
#include <string>
#include <unistd.h>


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
	std::vector<char const *>	_envp;


public:
	CgiHandler(HttpRequest const &request);

	bool	isCgiRequest() const; 
	bool	isCgiScript(std::string const &target);
	std::string	execCgiScript();
};

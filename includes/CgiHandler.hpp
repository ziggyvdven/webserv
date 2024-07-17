#pragma once

#include "../includes/HttpRequest.hpp"
#include <iostream>
#include <unistd.h>


class CgiHandler
{
private:
	HttpRequest	const	&_request;

	std::string			_htmlRoot;
	bool				_is_valid;
	std::string			_scriptName;
	std::string			_pathInfo;
	std::string			_queryString;

public:
	CgiHandler(HttpRequest const &request);
	std::string execCgi();

	bool	isCgiRequest() const; 
	bool	isCgiScript(std::string const &target);
	bool	execCgiScript();
};

#include "../includes/CgiHandler.hpp"

CgiHandler::CgiHandler(HttpRequest request)
{
	if (!request.isValid())
		throw std::exception();

	// Check if target is cgi
		// set _is_valid accordingly
		// stop if invalid
		_is_valid = true;

}

bool CgiHandler::isCgiRequest() const
{
	return _is_valid; 
}


std::string CgiHandler::execCgi()
{
	if(!isCgiRequest())
	{
		throw std::exception();
	}

	// send post data to stdin
	// execve the script

	// timeout

	return "<Cgi Response>";
}
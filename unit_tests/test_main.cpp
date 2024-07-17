#include <vector>
#include "../includes/utils.hpp"
#include <iostream>

#include "../includes/HttpRequest.hpp"
#include <unistd.h>
#include "../includes/CgiHandler.hpp"

void _execCGIScript(HttpRequest &request)
{
	(void) request;
}

int main()
{

	// Create Mock Request
	std::string http_str = \
	"POST /cgi-bin/echo HTTP/1.1\r\n"
	"\r\n"
	"<This is the full body content>";
	HttpRequest req(http_str);

	// Execute Mock Cgi
	_execCGIScript(req);

}
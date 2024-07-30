#include <iostream>
#include <sstream>
#include <strstream>
#include <unistd.h>
#include "../includes/utils.hpp"
#include <algorithm>
#include <vector>

#include "../includes/HttpRequest.hpp"

void test_request_edge()
{
	HttpRequest req;

	char *buf = NULL;
	req.parse(buf, 123);

}

void test_request_parsing()
{
	std::vector<char> line;
	(void) line;

	char in[] = "GET /index.html HTTP/1.1\r\n"
				"Host: some host\r\n"
				"SomeHeader: some other header\r\n"
				// "CONTENT-length: 123\r\n"
				"\r\n";
				// "<Body content>\r\n";
	
	std::istringstream http = std::istringstream(in);

	const int SIZE = 1;
	char r[SIZE];

	HttpRequest request;

	while (http.read(r, SIZE))
	{
		request.parse(r, SIZE);
	}
	request.parse(r, SIZE);

	request.print_request();
}

int main()
{
	test_request_edge();
	test_request_parsing();
}

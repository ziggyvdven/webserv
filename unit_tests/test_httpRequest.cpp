
#include "../includes/HttpRequest.hpp"

void test_request_edge()
{
	HttpRequest req;

	char *buf = NULL;
	req.parse(buf, 123);

}

void parse_str_request(char const *str_request, HttpRequest &request)
{

	const int SIZE = 2;
	char read_buf[SIZE];
	std::istringstream stream = std::istringstream(str_request);

	while (stream.read(read_buf, SIZE))
	{
		request.parse(read_buf, SIZE);
	}
}

void test_request_parsing()
{
	char str_request[] = "GET /index.html HTTP/1.1\r\n"
				"Host: some host\r\n"
				"SomeHeader: some other header\r\n"
				"CONTENT-length:    123 \r\n"
				"\r\n"
				"<Body content>\r\n";

	HttpRequest request;
	parse_str_request(str_request, request);

	request.print_request();
	if (request.getContentLength() > 0)
	{
		std::cout << "[PRINT BODY]" << std::endl;
	}
}

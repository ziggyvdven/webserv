
#include "../includes/HttpRequest.hpp"
#include "../includes/utils.hpp"

void test_request_edge()
{
	HttpRequest req;

	char *buf = NULL;
	req.parse(buf, 123);

}

void parse_str_request(std::string const &str_request, HttpRequest &request)
{

	const int SIZE = 1026;
	char read_buf[SIZE];
	std::istringstream stream = std::istringstream(str_request);

	while (stream.read(read_buf, SIZE) || stream.gcount() > 0)
	{
		request.parse(read_buf, stream.gcount());
	}

}

void test_request_parsing()
{
	std::string headers = "POST /index.html HTTP/1.1\r\n"
				"Host: some host\r\n"
				"SomeHeader: some other header\r\n";
	std::string body("Hello\0 World!");
	std::stringstream ss_request;

	ss_request	<< headers
					<< "content-length: " << body.size() << "\r\n"
					<< "\r\n"
					<< body;

	std::cout << ss_request.str()<< std::endl;

	HttpRequest request;
	parse_str_request(ss_request.str(), request);

	request.print_request();
	if (request.getContentLength() > 0)
	{
		print_vector_data(request.body());
	}
}

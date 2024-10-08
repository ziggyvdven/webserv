#include "../includes/HttpHandler.hpp"
#include "../includes/HttpRequest.hpp"
// #include "../includes/HttpResponse.hpp
#include "../includes/utils.hpp"
#include "../includes/Config.hpp"

#include <filesystem>

void parse_str_request(std::string const &str_request, HttpRequest &request);
void make_dummy_request(HttpRequest &request);




void test_request_handling() {
	HttpRequest request;
	Config conf("config/webserv.conf");

	HttpHandler http_handler(conf);

	make_dummy_request(request);
	std::string response = http_handler.buildResponse(request);

	std::cout << response << std::endl;
}






void make_dummy_request(HttpRequest &request) {
	std::string headers = "POST /cgi-bin/upload.py HTTP/1.1\r\n"
				"Host: localhost:4242\r\n"
				"SomeHeader: some other header\r\n";
	std::string body("Hello\0 World!");
	std::stringstream ss_request;
	ss_request	<< headers << "content-length: " << body.size() <<"\r\n"
				<< "\r\n" << body;

	std::cout << ss_request.str()<< std::endl;

	parse_str_request(ss_request.str(), request);
	request.print_request();
	if (request.getContentLength() > 0) {
		print_vector_data(request.body());
	}
}

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <stdexcept>
#include "utils.hpp"

class HttpRequest
{
private:
	std::string _raw;
	std::string	_method;
	std::string	_target;
	std::string	_version;
	std::map<std::string, std::string>	_headers;
	std::string	_body;
	bool _valid;
	std::istringstream _http_stream;


public:
	HttpRequest(std::string const &raw_str);

	std::string const &raw() const { return _raw; }
	std::string method() const;
	std::string const &target() const;
	std::string const &version() const;
	std::map<std::string, std::string> const &headers() const;
	std::string const &body() const;


private:
	void _parse_http_request();
	void _parse_request_line();
	void _validate_request_line(std::string const &request_line) const;

	void _parse_headers();
	bool _valid_header(std::string const &line) const;

// Debug
public:
	void print_request() const;
	void print_headers() const;

};

#pragma once 

#include <sstream>
#include <string>
#include <map>
#include "utils.hpp"
#include <algorithm>

class HttpRequest
{
private:
	std::vector<unsigned char>			_raw;
	std::string							_method;
	std::string							_target;
	std::string							_version;
	std::map<std::string, std::string>	_headers;
	std::vector<unsigned char>			_body;
	bool _valid;
	std::istringstream					_http_stream;


public:
	HttpRequest(std::vector<unsigned char> const &raw_str);
	std::vector<unsigned char> const	raw() const		{ return _raw; }
	std::string const	method() const	{ return _method; };
	std::string const	target() const	{ return _target; };
	std::string const	version() const	{ return _version; };
	std::vector<unsigned char> const	body() const	{ return _body; };
	bool				isValid() const	{ return _valid; };
	std::string const	getHeader(std::string const key) const;

private:
	void _parse_http_request();
	void _parse_request_line();
	void _validate_request_line(std::string const &request_line) const;

	void _parse_headers();
	void _add_header(std::string const key, std::string const value);
	bool _valid_header(std::string const &line) const;

	void _parse_body();


// Debug
public:
	void print_request() const;
	void print_headers() const;

};

#include "../includes/HttpRequest.hpp"

#include <iostream>
#include <regex>
#include <stdexcept>
// ========== ========== Constructor ========== ========== 
HttpRequest::HttpRequest(std::vector<unsigned char> const &raw_string)
	: _raw(raw_string), _valid(true), _http_stream(reinterpret_cast<char *>(_raw.data()))
{
	_parse_http_request();
}

// ========== ========== Validation ========== ========== 
void HttpRequest::_parse_http_request()
{
	_parse_request_line();
	_parse_headers();
	_parse_body();

}

// ========== ========== Getters ========== ========== 
std::string const HttpRequest::getHeader(std::string key) const
{
	toLowerCase(key);
	if (_headers.find(key) != _headers.end())
		return _headers.at(key);
	return "";
}

void HttpRequest::_add_header(std::string key, std::string value)
{
	value = trim(value);
	toLowerCase(key);

	_headers[key] = value;
}


void HttpRequest::_parse_request_line()
{
	std::string start_line;
	std::getline(_http_stream, start_line);

	try {
		std::istringstream iss(start_line);
		_validate_request_line(start_line);
		iss >> _method >> _target >> _version;
	}
	catch (std::exception const &e)
	{
		_valid = false;
	}
}

void HttpRequest::_validate_request_line(std::string const &request_line) const {

	std::regex re_request_line(
		"^(GET|POST|DELETE|HEAD|PUT)"
		" \\/\\S* "
		"HTTP\\/1.[1,3]\\r$");

	if (!std::regex_match(request_line, re_request_line))
	{
		throw std::exception();
	}
}

void HttpRequest::_parse_headers()
{
	std::string header_line;

	std::getline(_http_stream, header_line);
	while (!header_line.empty() && header_line != "\r")
	{
		// Skip invalid headers
		if (!_valid_header(header_line))
		{
			std::getline(_http_stream, header_line);
			continue;
		}

		// Add header to map
		int const pos = header_line.find(":");
		std::string key = header_line.substr(0, pos);
		std::string value = header_line.substr(pos + 1, header_line.size());
		_add_header(key, value);

		std::getline(_http_stream, header_line);
	}
}


bool HttpRequest::_valid_header(std::string const &line) const
{
	std::regex const re("^\\S+:[ ]?.*[ ]?\\r$");

	return std::regex_match(line, re);
}

void HttpRequest::_parse_body()
{
	std::vector<unsigned char>::iterator it;

	_body = std::vector<unsigned char>();
	if (this->getHeader("content-length").empty())
		return;

	try
	{
		// Fetch content lenght
		int content_length = std::atoi(this->getHeader("content-length").data());

		// Search for body start
		std::string delimiter = "\r\n\r\n";
		it = std::search(_raw.begin(), _raw.end(), delimiter.begin(), delimiter.end());
		if (it != _raw.end())
		{
			it += delimiter.size();
			_body = std::vector<unsigned char>(it, it + content_length);
		}
	} catch (std::invalid_argument)
	{
		return;
	}

	

}

void HttpRequest::print_request() const
{
	if (!_valid)
	{
		std::cerr << "\nInvalid Request." << std::endl;
		return;
	}

	std::cout << "method: " << _method << std::endl;
	std::cout << "Target: " << _target << std::endl;
	std::cout << "Version: " << _version << std::endl;

	std::cout << "\nHeaders: \n" << std::endl;
	print_headers();
}

void HttpRequest::print_headers() const
{
	std::map<std::string, std::string>::const_iterator cit;

	for(cit = _headers.begin(); cit != _headers.end(); ++cit)
		std::cout << cit->first << ": " << cit->second << std::endl;
}

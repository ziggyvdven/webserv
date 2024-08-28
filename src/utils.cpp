#include "../includes/utils.hpp"
#include <cctype>
#include <iomanip>
#include <iostream>

// trim from end of string (right)
std::string& rtrim(std::string& s, const char* t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from beginning of string (left)
std::string& ltrim(std::string& s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

// trim from both ends of string (right then left)
std::string& trim(std::string& s)
{
	const char* ws = " \t\n\r\f\v";

	return ltrim(rtrim(s, ws), ws);
}

void printStringWithNonPrintables(const std::string& str) {
    for (unsigned i=0; i < str.size(); ++i) {
		char c = str.data()[i];
        if (isprint(static_cast<unsigned char>(c))) {
            std::cout << c;
        } else {
            std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (static_cast<int>(c) & 0xFF);
        }
    }
}

void dump_stream(std::istream is)
{
	std::string line;

	int i = 0;
	do
	{
		std::getline(is, line);
		std::cout << "\n" << i << " - ";
		printStringWithNonPrintables(line);
		++i;
	} while (!line.empty());
	exit(0);
}

void toLowerCase(std::string & str)
{
	for (std::size_t i = 0; i < str.size(); ++i) {
		str[i] = std::tolower(static_cast<unsigned char>(str[i]));
	}
}

std::vector<std::string> ft_split(std::string str, std::string const &delim)
{
	std::vector<std::string> list;
	std::string token;

	size_t pos = 0;
		
	while ((pos = str.find(delim)) != std::string::npos && !delim.empty() && !str.empty())
	{
		token = str.substr(0, pos);
		str.erase(0, pos + delim.size());
		if (pos != 0)
			list.push_back(token);
	}
	if(!str.empty())
	{
		list.push_back(str);
	}

	return list;
}

long int time_since(std::chrono::steady_clock::time_point start)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::steady_clock::now() - start).count();
}

std::string const	getExtension(std::string const &file)
{
	size_t	dotPos = file.rfind('.');

	if (dotPos != std::string::npos)
	{
		return (file.substr(dotPos));
	}
	return ("");
}

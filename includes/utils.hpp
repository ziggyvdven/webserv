#pragma once

#include <vector>
#include <ctime>
#include <iostream>

std::string const	getExtension(std::string const &file);

template <typename T> T min(T a, T b)
{
	if (a < b)
		return a;
	return b;
}

template <typename T> T max(T a, T b)
{
	if (a > b)
		return a;
	return b;
}

std::string& trim(std::string& s);
std::string& ltrim(std::string& s, const char* t);
std::string& rtrim(std::string& s, const char* t);

void printStringWithNonPrintables(const std::string& str);
void toLowerCase(std::string & str);
std::vector<std::string> ft_split(std::string str, std::string const &delim);
int seconds_since(std::time_t start_time);
bool is_number(const std::string& s);

void insert_vector_data(std::vector<char> &dest, char *src, size_t len);
void print_vector_data(std::vector<char> const &buffer);
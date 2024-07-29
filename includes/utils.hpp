#pragma once

#include <vector>
#include <ctime>
#include <iostream>

#define Chrono long int

std::string& trim(std::string& s);
std::string& ltrim(std::string& s, const char* t);
std::string& rtrim(std::string& s, const char* t);

void printStringWithNonPrintables(const std::string& str);
void toLowerCase(std::string & str);
std::vector<std::string> ft_split(std::string str, std::string const &delim);
int time_since(clock_t start_time);

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
#include <iostream>

std::string& trim(std::string& s);
std::string& ltrim(std::string& s, const char* t);
std::string& rtrim(std::string& s, const char* t);

void printStringWithNonPrintables(const std::string& str);
void toLowerCase(std::string & str);
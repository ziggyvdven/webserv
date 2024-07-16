#include <vector>

std::string& trim(std::string& s);
std::string& ltrim(std::string& s, const char* t);
std::string& rtrim(std::string& s, const char* t);

void printStringWithNonPrintables(const std::string& str);
void toLowerCase(std::string & str);
std::vector<std::string> ft_split(std::string str, std::string const &delim);

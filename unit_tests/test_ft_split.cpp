#include "../includes/utils.hpp"
#include <iostream>

void test_split(std::string str, std::string delim, unsigned int expected_size)
{
	std::vector<std::string> list;
	list = ft_split(str, delim);
	if(list.size() == expected_size)
	{
		std::cout << "Test passed: " << str  << " " << delim << " => " << expected_size << std::endl; 
	}
	else {
		std::cerr << "Test Failed: " << str  << " " << delim << " => " << list.size() << " != " << expected_size << std::endl; 
		std::cout << "[List]: "<< std::endl;
		for(std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it)
			std::cout << *it << std::endl;
	}
}

void test_split_batch()
{
	test_split("", "", 0);
	test_split("hello", "", 1);
	test_split("Hello", " ", 1);
	test_split("Hello", "l", 2);
}

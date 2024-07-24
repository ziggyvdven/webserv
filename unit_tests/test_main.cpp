#include <ios>
#include <iostream>
#include <unistd.h>
#include <sys/fcntl.h>
#include "../includes/utils.hpp"

int main()
{
	int pid;
	int wstat;

	begin = std::chrono::steady_clock::now();
	pid = fork();
	if (pid == 0)
	{
		std::cout << "Child Porcess working ..." << std::endl;
		sleep(1);
		exit(0);
	}

	std::cout << "In Parent process" << std::endl;
	std::cout << "Waiting for child proces ..." << std::endl;

}

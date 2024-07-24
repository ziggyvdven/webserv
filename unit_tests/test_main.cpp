#include <iostream>
#include <fstream>

struct Record {
	char			name[40];
	unsigned short	age;
	float			gpa;
};

#include <iostream>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

int main()
{
	struct ifaddrs *interfaces = NULL;
	struct ifaddrs *ifa = NULL;

	if (getifaddrs(&interfaces) == -1)
	{
		perror("getifaddrs");
		return 1;
	}

	for (ifa = interfaces; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		int family = ifa->ifa_addr->sa_family;

		if (family == AF_INET  || family == AF_INET6)
		{
			char address[INET6_ADDRSTRLEN];
			void *addr_ptr = (family == AF_INET)
				? (void *)&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr
				: (void *)&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;

			inet_ntop(family, addr_ptr, address, sizeof(address));

			std::cout	<< "Interface: " << ifa->ifa_name
						<< "Address: " << address << std::endl;
		}
	}
	freeifaddrs(interfaces);
	return 0;
}
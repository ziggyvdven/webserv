#include "../includes/HttpResponse.hpp"

int	main(void)
{
	HttpResponse	response;

	//	Default Response
	std::cout << response.getResponse() << std::endl;
	std::cout << "------------------------ Response End --" << std::endl;

	//	Setting new content
	response.setContent("Hello World");
	std::cout << response.getResponse() << std::endl;
	std::cout << "------------------------ Response End --" << std::endl;

	//	Setting new header fields
	response.setHeader("CONnection", "close");
	response.setHeader("ETag", "Yo");
	std::cout << response.getResponse() << std::endl;
	std::cout << "------------------------ Response End --" << std::endl;

	return (0);
}

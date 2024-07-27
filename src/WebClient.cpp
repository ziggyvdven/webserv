#include "../includes/WebClient.hpp"

WebClient::WebClient(int accepted_connection)
	: Socket(accepted_connection)
{
}

void WebClient::send_data() {
	std::cout << "<Sending data to client>" << std::endl;

}

void WebClient::read_data() {
	std::cout << "<Reading data from client>" << std::endl;
	
}

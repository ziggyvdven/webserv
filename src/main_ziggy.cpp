# include "../includes/Common.hpp"
# include "../includes/Config.hpp"

using namespace std;
#define PORT 8080

int main(int argc, char **argv)
{
	if (argc == 2){
		string 	input(argv[1]);
		if (input.empty()){
			cerr << "ERROR [INPUT EMPTY]" << endl;
			return (1);
		}
		try {
			Config  config(input);
		}
		catch (exception &e)
		{
			cerr << "webserv: " << R << "ERROR" << END << "[" << e.what() << "]" << endl;
			return (1);
		}
	} else {
		cerr << "webserv: try 'webserv [configuration file]'" << endl;
		return (1);
	}
	return (0);



    // int server_fd, new_socket; long valread;
    // struct sockaddr_in address;
    // int addrlen = sizeof(address);
    
    // // Only this line has been changed. Everything is same.
    // string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 19\n\nwebserv is finished";
    
	// try {
	// 	// Creating socket file descriptor
	// 	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	// 		throw (runtime_error("in socket"));
		
	// 	address.sin_family = AF_INET;
	// 	address.sin_addr.s_addr = INADDR_ANY;
	// 	address.sin_port = htons( PORT );
		
	// 	memset(address.sin_zero, '\0', sizeof address.sin_zero);
		
		
	// 	if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0)
	// 		throw (runtime_error("in bind"));

	// 	if (listen(server_fd, 10) < 0)
	// 		throw (runtime_error("in listen"));

	// 	while(1)
	// 	{
	// 		cout << "+++++++ Waiting for new connection ++++++++" << endl;
	// 		if ((new_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&address), reinterpret_cast<socklen_t*>(&addrlen))) <0 )
	// 			throw (runtime_error("in accept"));
			
	//    		char buffer[30000] = {0};
	// 		valread = read(new_socket, buffer, 30000);
	// 		std::cout << buffer << std::endl;
	// 		write(new_socket , hello.c_str() , hello.length());
	// 		cout << "------------------Hello message sent-------------------" << endl;

	// 	}
	// 	close(new_socket);
    // 	close(server_fd);
	// }
	// catch (exception &e)
	// {
	// 	cout << "ERROR [" << e.what() << "]" << endl;
	// 	return (1);
	// }
	// return (0);
}
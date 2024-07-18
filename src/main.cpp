# include "../includes/Common.hpp"
# include "../includes/Config.hpp"

using namespace std;

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
			// cout << config.getServerConfig(80, "127.0.0.1/examplej") << endl;
			config.printConfig();
			Socket				socket0("localhost", 8080);
			Socket				socket1("127.0.0.1", 4242);
			std::vector<Socket>	socketList;

			socketList.push_back(socket0);
			socketList.push_back(socket1);

			WebServer			webServer(socketList);

			(void) argc;
			(void) argv;
			(void) envp;

			if (webServer.init() < 0)
			{	
				return (-1);
			}
			webServer.run();

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
}

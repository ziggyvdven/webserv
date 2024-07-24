# include "../includes/Common.hpp"
# include "../includes/Config.hpp"
# include "../includes/Socket.hpp"
# include "../includes/WebServer.hpp"

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
			// cout << config.getServerConfig(80, "127.0.0.1/example") << endl;
			// config.printConfig();
			std::vector<Socket>	socketList;
			for (unsigned i = 0; i < config.getNServers(); i++){
				Socket				socket(config.getServer(i).getHost(), config.getServer(i).getPort());
				socketList.push_back(socket);
			}

			WebServer			webServer(socketList, config);

			(void) argc;
			(void) argv;

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

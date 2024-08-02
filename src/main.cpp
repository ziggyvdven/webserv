# include "../includes/Common.hpp"
# include "../includes/Config.hpp"
# include "../includes/Socket.hpp"
# include "../includes/WebServer.hpp"
# include <csignal>

using namespace std;

int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
	if (argc == 2)
	{
		try 
		{
			Config  config(argv[1]);
			std::vector<Socket>	socketList;
			vector<short> portlist;
			config.printMsg(Y, "Setting up servers...");
			for (unsigned i = 0; i < config.getNServers(); i++)
			{
				if (std::find(portlist.begin(), portlist.end(), config.getServer(i).getPort()) == portlist.end())
				{
					config.printMsg(G, "Server [%s] created with port[%d] and host[%s]", config.getServer(i).getServerName().c_str(), config.getServer(i).getPort(), config.getServer(i).getHost().c_str());
					Socket				socket(config.getServer(i).getHost(), config.getServer(i).getPort());
					socketList.push_back(socket);
					portlist.push_back(config.getServer(i).getPort());
				}
			}
			WebServer			webServer(socketList, config);
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
	}
	else {
		cerr << "webserv: invalid argument try './webserv [configuration file]'" << endl;
		return (1);
	}
	return (0);
}

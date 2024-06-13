#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "Common.hpp"
# include "Config.hpp"
# include <vector>
# include <string>
# include <map>
# include <unordered_set>

using namespace std;

# define BOLD "\033[1;37m"
# define END "\033[0m"
# define G "\033[0;32m"
# define B "\033[0;34m"
# define R "\033[0;31m"

class Config;

class ConfigServer {
public:
	ConfigServer(vector<string> const & block, Config & config);
	ConfigServer ( ConfigServer const & src);
	~ConfigServer( void );

	ConfigServer &		operator=( ConfigServer const & rhs );


private:
	// ConfigServer();
	vector<string> 			_Block;
	Config &				_Config;
	// int					_Port;
	// string				_Host;
	// string				_ServerName;
	// long					_ClientMaxBodySize;
	// map<short, string>	_ErrorPages;
	// bool					_AutoIndex;



};

#endif /* ********************************************************** SERVER_H */
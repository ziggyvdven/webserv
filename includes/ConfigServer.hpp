#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "Common.hpp"
# include "Config.hpp"
# include <vector>
# include <string>
# include <map>
# include <unordered_set>
# include <arpa/inet.h>

using namespace std;

# define BOLD "\033[1;37m"
# define END "\033[0m"
# define G "\033[0;32m"
# define B "\033[0;34m"
# define R "\033[0;31m"

class Config;

class ConfigServer {
public:
	ConfigServer(vector<pair<string, unsigned> > const & conf, Config & config);
	ConfigServer ( ConfigServer const & src);
	~ConfigServer( void );

	ConfigServer &		operator=( ConfigServer const & rhs );

	void				Parseline(pair<string, unsigned> & linepair, string& line);
	void				ParseListen(pair<string, unsigned> & linepair);
	void				ParseServerName(pair<string, unsigned> & linepair);

	unsigned short		getPort() const;
	string				getHost() const;
	string				getServerName() const;

	bool				ValidatePort(string& line, string N) const;


private:
	vector<pair<string, unsigned> > 	_Block;
	Config&								_Config;
	unsigned short						_Port;
	string								_Host;
	string								_ServerName;
	// long								_ClientMaxBodySize;
	// map<short, string>				_ErrorPages;
	// bool								_AutoIndex;



};

std::ostream &			operator<<( std::ostream & o, ConfigServer const & i );

#endif /* ********************************************************** SERVER_H */
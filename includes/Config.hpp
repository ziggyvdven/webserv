#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Common.hpp"
# include "ConfigServer.hpp"
# include <regex>
# include <vector>
# include <string>
# include <unordered_set>

using namespace std;

# define BOLD "\033[1;37m"
# define END "\033[0m"
# define G "\033[0;32m"
# define B "\033[0;34m"
# define R "\033[0;31m"

const string CONFIG_FOLDER = "config";

class ConfigServer;

class Config {
public:
	Config ( string const & input );
	Config ( Config const & src);
	~Config( void );

	Config &					operator=( Config const & rhs );

	int							CreateConfigServer( );
	unordered_set<string>&		getDirectives();
	string						getFilename() const;

private:
	Config ( void );
	ifstream					_ConfigFile;
	string						_Filename;
	unordered_set<string>		_Directives;
	vector<ConfigServer>		_ConfigServers;
	
};

#endif /* ************************************************ BITCOINEXCHANGE_H */	
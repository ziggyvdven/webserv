#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Common.hpp"
# include "ConfigServer.hpp"
# include <regex>
# include <vector>
# include <string>
# include <unordered_set>
# include <utility>
# include <cstdlib>

# define NUM_DIRECTIVES 14

const string CONFIG_FOLDER = "config";

class ConfigServer;

class Config {
public:
	Config ( string const & input );
	Config ( Config const & src);
	~Config( void );

	Config &					operator=( Config const & rhs );

	unordered_set<string>&		getDirectives( void );
	unsigned					getLinenumber( void ) const;
	string						getFilename( void ) const;
	unsigned					getNServers( void ) const;
	vector<ConfigServer>&		getConfigServers( void );
	ConfigServer&				getServer(size_t index);
	ConfigServer&				getServerConfig(string const & host, string const & target);
	void						printConfig( void );
	void   						printMsg(const char *color, const char* msg, ...);
	std::string 				getCurrTime( void );

private:
	void						incrementLinenumber( void );
	int							CreateConfigServer( void );
	void 						init_directives( void );
	void						check_double_configs( void );

	unsigned					_Linenumber;
	unsigned					_NServers;
	ifstream					_ConfigFile;
	string						_Filename;
	unordered_set<string>		_Directives;
	vector<ConfigServer>		_ConfigServers;
	
};

#endif /* ********************************************************* CONFIG_HPP*/	
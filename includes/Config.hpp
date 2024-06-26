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

const string CONFIG_FOLDER = "config";

class ConfigServer;

class Config {
public:
	Config ( string const & input );
	Config ( Config const & src);
	~Config( void );

	Config &					operator=( Config const & rhs );

	int							CreateConfigServer( );
	void 						init_directives( void );


	unordered_set<string>&		getDirectives();
	unsigned					getLinenumber() const;
	string						getFilename() const;
	void						incrementLinenumber();

private:
	unsigned					_Linenumber;
	ifstream					_ConfigFile;
	string						_Filename;
	unordered_set<string>		_Directives;
	vector<ConfigServer>		_ConfigServers;
	
};

#endif /* ************************************************ BITCOINEXCHANGE_H */	
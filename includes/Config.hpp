#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Common.hpp"
# include <regex>
# include <vector>
# include <string>

using namespace std;

# define BOLD "\033[1;37m"
# define END "\033[0m"
# define G "\033[0;32m"
# define B "\033[0;34m"
# define R "\033[0;31m"


class Config {
public:
	Config ( string const & input );
	Config ( Config const & src);
	~Config( void );

	Config &		operator=( Config const & rhs );


private:
	Config ( void );
	ifstream		_ConfigFile;	

};

#endif /* ************************************************ BITCOINEXCHANGE_H */
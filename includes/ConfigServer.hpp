#ifndef SERVER_HPP
# define SERVER_HPP

# include "Common.hpp"
# include <vector>
# include <string>

using namespace std;

# define BOLD "\033[1;37m"
# define END "\033[0m"
# define G "\033[0;32m"
# define B "\033[0;34m"
# define R "\033[0;31m"


class ConfigServer {
public:
	ConfigServer ( vector<string> const & conf );
	ConfigServer ( ConfigServer const & src);
	~ConfigServer( void );

	ConfigServer &		operator=( ConfigServer const & rhs );


private:
	ConfigServer ( void );
	vector<string> 			_block;
	// struct sockaddr_in 		_addr;
	// int 					_fd;

};

#endif /* ********************************************************** SERVER_H */
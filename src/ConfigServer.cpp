#include "../includes/ConfigServer.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
ConfigServer::ConfigServer(): _block(){
	// cout << G << "ConfigServer Default constructor called" << END << std::endl;
}

ConfigServer::ConfigServer(vector<string> const & conf): _block(conf){
	// cout << G << "ConfigServer constructor by param called" << END << std::endl;
	cout << endl << "SERVER CONFIGURATION" << endl;
	for (vector<string>::iterator it = _block.begin(); it != _block.end(); ++it)
		cout << *it << endl;
}

ConfigServer::ConfigServer( const ConfigServer & src ){
	// std::cout << G << "ConfigServer Copy constructor called" << END << std::endl;
	*this = src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ConfigServer::~ConfigServer(){
	// std::cout << R << "ConfigServer Destructor called" << END << std::endl;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ConfigServer &				ConfigServer::operator=( ConfigServer const & rhs )
{
	// std::cout << "ConfigServer Copy assignment operator called" << std::endl;
	if ( this != &rhs )
	{

	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */
#include "../includes/Config.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Config::Config() {
	cout << G << "Config Default constructor called" << END << std::endl;
}

Config::Config(string const & input) {
	cout << G << "Config Default constructor called" << END << std::endl;
	regex 		config_filename("[a-zA-Z_0-9]+\\.conf");
	string 		directives[] = {"html", "events", "server"};
	string		line;

	if (!regex_match(input, config_filename))
		throw(runtime_error(input + " is not a valid .conf file")); 
	_ConfigFile.open(input);
	if (!_ConfigFile.good())
		throw(runtime_error("unable to open: " + input));
	while (getline(_ConfigFile, line)){
		cout << line << endl;
	}
	
}

Config::Config( const Config & src ){
	std::cout << G << "Config Copy constructor called" << END << std::endl;
	*this = src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Config::~Config(){
	std::cout << R << "Config Destructor called" << END << std::endl;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Config &				Config::operator=( Config const & rhs )
{
	std::cout << "Config Copy assignment operator called" << std::endl;
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
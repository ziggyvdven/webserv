#include "../includes/ConfigGlobal.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Config::Config() {
}

Config::Config(string const & input) {
	regex 			config_filename("[a-zA-Z_0-9]+\\.conf");
	string 			directives[] = {"events", "server"};
	regex			server_reg("^server\\s*\\{");
	string			line;

	if (!regex_match(input, config_filename))
		throw(runtime_error(input + " is not a valid .conf file"));
	_Filename = input;
	_ConfigFile.open(input);
	if (!_ConfigFile.good())
		throw(runtime_error("unable to open: " + input));
	while (getline(_ConfigFile, line)){
		if (regex_match(line, server_reg)){
			CreateConfigServer( );
		}
		else if (!line.empty())
			throw (runtime_error(input + " invalid entry in config file: " + line));
	}	
}

Config::Config( const Config & src ){
	// std::cout << G << "Config Copy constructor called" << END << std::endl;
	*this = src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Config::~Config(){
	// std::cout << R << "Config Destructor called" << END << std::endl;
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Config & Config::operator=( Config const & rhs )
{
	// std::cout << "Config Copy assignment operator called" << std::endl;
	if ( this != &rhs )
	{

	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

int	Config::CreateConfigServer( ){

	string 			whitespace = " \t";
	regex			server_reg("^server\\s*\\{");
	vector<string>	block;
	string			line;
	long			opening_brackets = 1;
	long			closing_brackets = 0;

	while (getline(_ConfigFile, line))
	{
		if (line.find("}") != string::npos)
			closing_brackets++;
		if (line.find("{") != string::npos)
			opening_brackets++;
		if (opening_brackets == closing_brackets)
			break;
		if (regex_match(line, server_reg))
			throw (runtime_error(_Filename + " missing closing bracket for server configuration"));	
		if (!line.empty() && line[line.find_first_not_of(whitespace)] != '#')
			block.push_back(line);
	}
	_ConfigServers.push_back(ConfigServer(block));
	return (0);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */
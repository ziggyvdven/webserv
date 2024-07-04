#include "../includes/ConfigServer.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
ConfigServer::ConfigServer(vector<pair<string, unsigned> > const & conf, Config & config): _Block(conf), _Config(config){
	// cout << G << "ConfigServer constructor by param called" << END << std::endl;
	cout << endl << "SERVER CONFIGURATION" << endl;
	// _func[0] = &ConfigServer::parse_port;


	string 			whitespace = " \t";
	string			line;

	for (vector<pair<string, unsigned> >::iterator it = _Block.begin(); it != _Block.end(); ++it){
		line = it->first;
		// cut the first word from the string
		if (!line.empty() && line.find_first_not_of(whitespace) != string::npos){
			size_t start = line.find_first_not_of(whitespace);
			size_t end = line.find_first_of(whitespace, start) - 1;
			line = line.substr(start, end);
		}
		// cout << line << endl;
		unordered_set<string>		directives = _Config.getDirectives();
		if (directives.find(line) == directives.end()){
			throw (runtime_error("unknown directive \"" + line + "\" in " + _Config.getFilename() + ":" + to_string(it->second)));
		}
		else
			Parseline(*it);
	}
}

ConfigServer::ConfigServer( const ConfigServer & src ): _Config(src._Config){
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

ConfigServer &	ConfigServer::operator=( ConfigServer const & rhs )
{
	// std::cout << "ConfigServer Copy assignment operator called" << std::endl;
	if ( this != &rhs )
	{
		_Config = rhs._Config;
	}
	return *this;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	ConfigServer::Parseline(pair<string, unsigned> &linepair){

	(void) linepair;
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */
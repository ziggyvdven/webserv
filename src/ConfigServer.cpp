#include "../includes/ConfigServer.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
ConfigServer::ConfigServer(vector<pair<string, unsigned> > const & conf, Config & config): _Block(conf), _Config(config), 
_Port(0){
	// cout << G << "ConfigServer constructor by param called" << END << std::endl;
	// cout << endl << "SERVER CONFIGURATION" << endl;
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
			Parseline(*it, line);
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

void	ConfigServer::Parseline(pair<string, unsigned> &linepair, string line){
    string	Directives[NUM_DIRECTIVES] = {
        "listen", "host", "server_name", "client_max_body_size", 
        "error_page", "location", "allow_methods", "autoindex", 
        "index", "root", "cgi_path", "cgi_ext"
    };
	for (int i = 0; i <= NUM_DIRECTIVES; i++){
		if (line == Directives[i])
		{
			switch (i){
				case 0:
					ParseListen(linepair);
				break;
			}
			return ;
		}
	}
}

void	ConfigServer::ParseListen(pair<string, unsigned> & linepair){
	regex 	port_line("[[:space:]]*listen[[:space:]]*[0-9]+;[[:space:]]*");
	regex 	host_line("[[:space:]]*listen[[:space:]]*[0-9]+.[0-9]+.[0-9]+.[0-9]+:[0-9]+;[[:space:]]*");
	string  line = linepair.first;
	// cout << linepair.first << endl;
	if (regex_match(line, port_line)){
		size_t start = line.find_first_of("0123456789");
		size_t end = line.find_first_not_of("0123456789", start);
		line = line.substr(start, end - start);
		try{
			unsigned port = stoi(line);
			if (port > 0 && port <= 65535)
				_Port = port;
		}
		catch (exception &e)
		{
			throw (runtime_error("invalid port in \"" + line + "\" of the \"listen\" directive in /config/webserv.conf:" + to_string(linepair.second)));
		}
	}
	else if (regex_match(line, host_line)){
		cout << "Port and host";
	}
	else 
		throw (runtime_error("Port or host not found in the \"listen\" directive in /config/webserv.conf:" + to_string(linepair.second)));
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */
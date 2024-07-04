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

ConfigServer::ConfigServer( const ConfigServer & src ):  _Block(src._Block), _Config(src._Config), _Port(src._Port), _Host(src._Host),
_ServerName(src._ServerName){
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
		_Port = rhs._Port;
		_Host = rhs._Host;
		_Block = rhs._Block;
		_ServerName = rhs._ServerName;
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, ConfigServer const & i )
{
	o << "______________SERVER_CONFIG______________________________" << endl;
	o << "SEVER NAME| " << i.getServerName() << endl;
	o << "      HOST| " << i.getHost() << endl;
	o << "      PORT| " << i.getPort() << endl;
	o << "AUTO-INDEX| " << endl;
	o << "MAX C.BODY| " << endl;
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	ConfigServer::Parseline(pair<string, unsigned> &linepair, string& line){
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
				case 1:
					continue;
				case 2:
					ParseServerName(linepair);
				break;
			}
			return ;
		}
	}
}

bool	ConfigServer::ValidatePort(string& line, string N)const{

	try{
			int port = stoi(line);
			if (port <= 0 || port > 65535)
				throw (runtime_error("invalid port in \"" + line + "\" of the \"listen\" directive in /config/webserv.conf:" + N));
			return (true);
	}
	catch (exception &e){
		throw (runtime_error("invalid port in \"" + line + "\" of the \"listen\" directive in /config/webserv.conf:" + N));
	}
	return (false);
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
		if (ValidatePort(line, to_string(linepair.second)))
			_Port = stoi(line);;
	}
	else if (regex_match(line, host_line)){
		size_t start = line.find_first_of("0123456789");
		size_t end = line.find_first_of(":", start);
		string host = line.substr(start, end - start);
		struct sockaddr_in sa;
		int result = inet_pton(AF_INET, host.c_str(), &(sa.sin_addr));
		if (!result)
			throw (runtime_error("invalid host in \"" + host + "\" of the \"listen\" directive in /config/webserv.conf:" + to_string(linepair.second)));
		start = end + 1;
		end = line.find_first_not_of("0123456789", start);
		line = line.substr(start, end - start);
		if (ValidatePort(line, to_string(linepair.second)))
			_Port = stoi(line);
		_Host = host;
	}
	else 
		throw (runtime_error("Port or host not found in the \"listen\" directive in /config/webserv.conf:" + to_string(linepair.second)));
}

void	ConfigServer::ParseServerName(pair<string, unsigned> & linepair){
	regex 	servername_line("[[:space:]]*server_name[[:space:]]*[A-Za-z0-9]+;[[:space:]]*");
	string  line = linepair.first;

	if (regex_match(line, servername_line)){
		size_t start = line.find("server_name") + 11;
		size_t end = line.find(";") - 2;
		string servername = line.substr(line.find_first_not_of(" \t", start), end - start);
		_ServerName = servername;
	}
	else 
		throw (runtime_error("Server name not found in the \"server_name\" directive in /config/webserv.conf:" + to_string(linepair.second)));
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

unsigned short		ConfigServer::getPort() const{
	return (this->_Port);
}

string				ConfigServer::getHost() const{
	return (this->_Host);
}

string				ConfigServer::getServerName() const{
	return (this->_ServerName);
}

/* ************************************************************************** */
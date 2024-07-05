#include "../includes/ConfigServer.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
ConfigServer::ConfigServer(vector<pair<string, unsigned> > const & conf, Config & config): _Block(conf), _Config(config), 
_Port(0), _ClientMaxBodySize(0){
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
_ServerName(src._ServerName), _ClientMaxBodySize(src._ClientMaxBodySize), _ErrorPages(src._ErrorPages){
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
		_ClientMaxBodySize = rhs._ClientMaxBodySize;
		_ErrorPages = rhs._ErrorPages;
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
	if (i.getClientMaxBodySize() >= 1048576){
		long long max = i.getClientMaxBodySize() >> 20;
		o << "MAX C.BODY| " << max << " MegaBytes" << endl;
	}
	else
		o << "MAX C.BODY| " << i.getClientMaxBodySize() << " Bytes" << endl;
	o << "ERROR-PAGES| ";
	for (map<short, string>::iterator it = _ErrorPages.begin(); it != _ErrorPages.end(); ++it) {
        o << it->first << ": " << it->second << " ";
    }
	o << endl;
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
					break;
				case 1:
					ParseHost(linepair);
					break;
				case 2:
					ParseServerName(linepair);
					break;
				case 3:
					ParseClientMaxBodySize(linepair);
					break;
				case 4:
					ParseErrorPage(linepair);
					break;
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
				throw (runtime_error("invalid port in \"" + line + "\" of the \"listen\" directive in " + _Config.getFilename() + N));
			return (true);
	}
	catch (exception &e){
		throw (runtime_error("invalid port in \"" + line + "\" of the \"listen\" directive in " + _Config.getFilename() + N));
	}
	return (false);
}		

void	ConfigServer::ParseListen(pair<string, unsigned> & linepair){
	regex 	port_line("[[:space:]]*listen[[:space:]]*[0-9]+;[[:space:]]*");
	regex 	host_line("[[:space:]]*listen[[:space:]]*[0-9]+.[0-9]+.[0-9]+.[0-9]+:[0-9]+;[[:space:]]*");
	string  line = linepair.first;

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
			throw (runtime_error("invalid host in \"" + host + "\" of the \"listen\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
		start = end + 1;
		end = line.find_first_not_of("0123456789", start);
		line = line.substr(start, end - start);
		if (ValidatePort(line, to_string(linepair.second)))
			_Port = stoi(line);
		_Host = host;
	}
	else 
		throw (runtime_error("Port or host not found in the \"listen\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseHost(pair<string, unsigned> & linepair){
	//Parses the host directive and uses the function inet_pton to check if the adressis valid. inet_pton returns
	regex 	host_line("[[:space:]]*host[[:space:]]*[0-9]+.[0-9]+.[0-9]+.[0-9]+;[[:space:]]*");
	string  line = linepair.first;

	if (regex_match(line, host_line)){
		size_t start = line.find_first_of("0123456789");
		size_t end = line.find_first_of(";");
		string host = line.substr(start, end - start);
		struct sockaddr_in sa;
		int result = inet_pton(AF_INET, host.c_str(), &(sa.sin_addr));
		if (!result)
			throw (runtime_error("invalid host in \"" + host + "\" of the \"host\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
		_Host = host;
	}
	else 
		throw (runtime_error("Host not found in the \"host\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseServerName(pair<string, unsigned> & linepair){
	//Parses the servername only takes alphanumeric characters and upper and lower case
	regex 	servername_line("[[:space:]]*server_name[[:space:]]*[A-Za-z0-9-_]+;[[:space:]]*");
	string  line = linepair.first;

	size_t start = line.find("server_name") + 11;
	size_t end = line.find(";") - 2;
	string servername = line.substr(line.find_first_not_of(" \t", start), end - start);
	if (regex_match(line, servername_line))
		_ServerName = servername;
	else 
		throw (runtime_error("Invalid server name \"" + servername + "\" in the \"server_name\" directive in /config/webserv.conf:" + to_string(linepair.second)));
}

void	ConfigServer::ParseClientMaxBodySize(pair<string, unsigned> & linepair){
	//Sets the maximum allowed size of the client request body. If the size in a request exceeds the configured value, the 413 (Request Entity Too Large) error is returned to the client. Setting size to 0 disables checking of client request body size.
	regex 	client_max_body_size_line("[[:space:]]*client_max_body_size[[:space:]]*[0-9]+[m]*;[[:space:]]*");
	string  line = linepair.first;
	long 	clientmaxbodysize = 0;

	size_t start = line.find("client_max_body_size") + 20;
	size_t end = line.find(";") - 1;
	string client_max_body_size = line.substr(line.find_first_not_of(" \t", start), end - start);
	if (regex_match(line, client_max_body_size_line)){
		
		if (client_max_body_size.find("m") != string::npos){
			client_max_body_size.pop_back();
			try{
				clientmaxbodysize = stoi(client_max_body_size);
			}
			catch (exception &e){
				throw (runtime_error("\"client_max_body_size\" directive invalid value in " + _Config.getFilename() + ":" + to_string(linepair.second)));
			}
			if (clientmaxbodysize > 1024)
					throw (runtime_error("Client_max_body_size surpasses maximum value of 1024mb in " + _Config.getFilename() + ":" + to_string(linepair.second)));
			_ClientMaxBodySize = clientmaxbodysize << 20;
		}
		else{
			_ClientMaxBodySize = stoi(client_max_body_size);
		}
	}
	else 
		throw (runtime_error("Invalid client_max_body_size \"" + client_max_body_size + "\" in the \"client_max_body_size\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

string ConfigServer::trim(const string& str) {
	// trims the leading whitespaces of the error code tokens.
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

void	ConfigServer::ParseErrorCodes(vector<std::string>& tokens, unsigned const & linenumber) const{
	//Checks if error codes are only digits and between 300 and 599;
	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it){
		if (it->find_first_not_of("0123456789") != string::npos)
			throw (runtime_error("invalid value \"" + *it + "\" in the \"error_page\" directive in " + _Config.getFilename() + ":" + to_string(linenumber)));
		unsigned errorcode = stoi(*it);
		if (errorcode < 300 || errorcode > 599)
			throw (runtime_error("Error code value \"" + *it + "\" must be between 300 and 599 in " + _Config.getFilename() + ":" + to_string(linenumber)));
	}
}

void	ConfigServer::ParseErrorPage(pair<string, unsigned> & linepair){
	//Defines the URI that will be shown for the specified errors. A uri value can contain variables.
	string  line = linepair.first;

	//cut the error_page of the line;
	size_t start = line.find("error_page") + 10;
	size_t end = line.find(";") - 1;
	string errorpages = line.substr(line.find_first_not_of(" \t", start), end);
	// cout << errorpages << endl;
	// if (regex_match(line, error_page_line)){
		istringstream iss(errorpages);
    	vector<std::string> tokens;
    	string token;
    	while (iss >> token) 
        	tokens.push_back(trim(token));
		string page = tokens.back();
		tokens.pop_back();
		ParseErrorCodes(tokens, linepair.second);
		for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
			_ErrorPages[stoi(*it)] = page;
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

unsigned short	ConfigServer::getPort() const{
	return (this->_Port);
}

string	ConfigServer::getHost() const{
	return (this->_Host);
}

string	ConfigServer::getServerName() const{
	return (this->_ServerName);
}

long long	ConfigServer::getClientMaxBodySize() const{
	return (this->_ClientMaxBodySize);
}

string	ConfigServer::getErrorPage(short const & errorcode) const{
	try {
		string errorpage = _ErrorPages.at(errorcode);
		return errorpage;
	}
	catch (const std::out_of_range& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
	return ("");
}

map<short, string>*	ConfigServer::getErrorPageMap() const{
	return (this->_ErrorPages);
}

/* ************************************************************************** */
#include "../includes/Config.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Config::Config(string const & input) : _Linenumber(0), _NServers(0){
	regex 			config_filename(CONFIG_FOLDER + "\\/[a-zA-Z_0-9]+\\.conf");
	regex			server_reg("^server\\s*\\{");
	string			line;
	string 			whitespace = " \t";
	
	// check the input extension .conf
	if (!regex_match(input, config_filename))
		throw(runtime_error(input + " is not a valid .conf file"));
	init_directives();
	_Filename = input;
	// open the input file
	_ConfigFile.open(input);
	if (!_ConfigFile.good())
		throw(runtime_error("unable to open: " + input));
	// look for "server {""
	while (getline(_ConfigFile, line)){
		_Linenumber++;
		if (regex_match(line, server_reg))
			CreateConfigServer( );
		else if (!line.empty() && line[line.find_first_not_of(whitespace)] != '#' && line.find_first_not_of(whitespace) != string::npos)
			throw (runtime_error("unknown directive \"" + line + "\" in " + _Filename + ":" + to_string(_Linenumber)));
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
	for (vector<ConfigServer>::iterator it = _ConfigServers.begin(); it != _ConfigServers.end(); ++it) {
		map<string, ConfigServer*>	routes = it->getRoutes();
		for (map<string, ConfigServer*>::iterator it = routes.begin(); it != routes.end(); ++it)
		{
			delete it->second;
		}
	}
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
// function to create a server setting block from the opening { to the closing }
	string 							whitespace = " \t";
	regex							server_reg("^server\\s*\\{");
	vector<pair<string, unsigned> >	block;
	string							line;
	pair<string, unsigned> 			linepair;
	long							opening_brackets = 1;
	long							closing_brackets = 0;

	while (getline(_ConfigFile, line))
	{
		_Linenumber++;
		if (line[line.find_first_not_of(whitespace)] != '#' && line.find("}") != string::npos)
			closing_brackets++;
		if (line[line.find_first_not_of(whitespace)] != '#' && line.find("{") != string::npos)
			opening_brackets++;
		if (opening_brackets == closing_brackets)
			goto match;
		if (regex_match(line, server_reg))
			throw (runtime_error(_Filename + " missing closing bracket for server configuration"));	
		if (!line.empty() && line[line.find_first_not_of(whitespace)] != '#' && line.find_first_not_of(whitespace) != string::npos)
		{
			linepair.first = line;
			linepair.second = _Linenumber;
			block.push_back(linepair);
		}
	}
	throw (runtime_error(_Filename + " missing closing bracket for server configuration"));	
	match:
	ConfigServer server(*this);
	_ConfigServers.push_back(server);
	_ConfigServers.back().Init(block);
	_NServers++;
	return (0);

}

void Config::init_directives( void ){
// Creates a set of possible directives
	_Directives.insert("listen");
	_Directives.insert("host");
	_Directives.insert("server_name");
	_Directives.insert("client_max_body_size");
	_Directives.insert("error_page");
	_Directives.insert("location");
	_Directives.insert("allow_methods");
	_Directives.insert("autoindex");
	_Directives.insert("index");
	_Directives.insert("root");
	_Directives.insert("cgi_bin");
	_Directives.insert("cgi_ext");
	_Directives.insert("return");
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

unordered_set<string>&	Config::getDirectives(){
	return (_Directives);
}

string	Config::getFilename() const{
	return (this->_Filename);
}

unsigned	Config::getLinenumber() const{
	return (_Linenumber);
}

unsigned	Config::getNServers() const{
	return (_NServers);
}

void	Config::incrementLinenumber(){
	_Linenumber++;
}

vector<ConfigServer>&	Config::getConfigServers(){
	return(_ConfigServers);
}

ConfigServer&	Config::getServer(size_t index){

	try{
		return (_ConfigServers.at(index));
	}
	catch (exception &e){
		cerr << "GetServer: index out of bounds" << endl;
		exit (1);
	}
}

ConfigServer&	Config::getServerConfig(unsigned const & port, string const & host){

	string mhost = host;

	if (mhost.find("localhost")){
		size_t pos = mhost.find("localhost");
		if (pos != string::npos)
			mhost.replace(pos, 10, "127.0.0.1");
	}
	for (vector<ConfigServer>::iterator it = _ConfigServers.begin(); it != _ConfigServers.end(); ++it){
		if (it->getPort() == port && host.find(it->getHost()) != string::npos){
			if (it->getRedirect().first != 0)
				return (*it);
			try {
				return (*it->getRoutes().at(mhost));
			}
			catch(exception &e){
				cout << "getServerConfig: route not found returning default settings for server." << endl;
				return (*it);
			}
		}
	}
	throw (runtime_error("getServerConfig: Port/Host not found in " + _Filename));
}

void	Config::printConfig(){

	for (vector<ConfigServer>::iterator it = _ConfigServers.begin(); it != _ConfigServers.end(); ++it){
		cout << *it << endl;
		if (!it->getRoutes().empty())
		{
			map<string, ConfigServer*>	Routes = it->getRoutes();
			for (map<string, ConfigServer*>::iterator it2 = Routes.begin(); it2 != Routes.end(); ++it2){
				cout << *it2->second << endl;
			}
		}
	}
}


/* ************************************************************************** */
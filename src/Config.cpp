#include "../includes/Config.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Config::Config(string const & input) :_Linenumber(0), _NServers(0){
	regex 			config_filename(CONFIG_FOLDER + "\\/([a-zA-Z_0-9]+\\/)*[a-zA-Z_0-9]+\\.conf");
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
	if (_ConfigFile.peek() == std::ifstream::traits_type::eof())
		throw(runtime_error("Configuration file : " + input + ", is empty"));
	// look for "server {""
	while (getline(_ConfigFile, line)){
		_Linenumber++;
		if (regex_match(line, server_reg))
			CreateConfigServer( );
		else if (!line.empty() && line[line.find_first_not_of(whitespace)] != '#' && line.find_first_not_of(whitespace) != string::npos)
			throw (runtime_error("unknown directive \"" + line + "\" in " + _Filename + ":" + to_string(_Linenumber)));
	}
	if (_ConfigServers.empty())
		throw (runtime_error("No directives found in " + _Filename));
}

Config::Config( const Config & src ){
	// std::cout << G << "Config Copy constructor called" << END << std::endl;
	_Linenumber = src._Linenumber;
	_NServers = src._NServers;
	_Filename = src._Filename ; 
	_Directives = src._Directives;
	_ConfigServers =  src._ConfigServers;
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
	_ConfigFile.close();
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Config & Config::operator=( Config const & rhs )
{
	// std::cout << "Config Copy assignment operator called" << std::endl;
	if ( this != &rhs )
	{
			_Linenumber = rhs._Linenumber;
			_NServers = rhs._NServers;
			_Filename = rhs._Filename ; 
			_Directives = rhs._Directives;
			_ConfigServers =  rhs._ConfigServers;
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
	check_double_configs();
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
	_Directives.insert("upload_dir");
}

void Config::check_double_configs( void ){

	for (unsigned i = 0; i < _NServers - 1; i++){
		if (_ConfigServers.back().getPort() == _ConfigServers[i].getPort())
			if (_ConfigServers.back().getHost() == _ConfigServers[i].getHost())
				if (_ConfigServers.back().getServerName() == _ConfigServers[i].getServerName())
					throw (runtime_error("Multiple servers in " + _Filename + " with the same settings."));
	}

}

void    Config::printMsg(const char *color, const char* msg, ...)
{
    char        output[8192];
    va_list     args;
    int         n;

	va_start(args, msg);
	n = vsnprintf(output, 8192, msg, args);
	std::string date = getCurrTime();
	std::cout << color << getCurrTime() << output << END << std::endl;   
	va_end(args);
}

std::string Config::getCurrTime()
{
    tzset();
    char date[1000];
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    tm.tm_hour = tm.tm_hour - 4;
    strftime(date, sizeof(date), "[%Y-%m-%d  %H:%M:%S]   ", &tm);
    return (std::string(date));
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

ConfigServer&	Config::getServerConfig(string const & host, string const & target){

	string  			mhost = host;
	unsigned short 		port;
	size_t				match = 0;
	ConfigServer* 		ptr = NULL;

	if (mhost.find("localhost") != string::npos || host == "localhost")
	{
		size_t pos = mhost.find("localhost");
		if (pos != string::npos)
			mhost.replace(pos, 9, "127.0.0.1");
	}
	if (mhost.find(":") != string::npos)
	{
		port = stoi(mhost.substr(mhost.find(":") + 1));
		mhost = mhost.substr(0, mhost.find(":"));
	}
	else
	{
		port = 80;
	}
	for (vector<ConfigServer>::iterator it = _ConfigServers.begin(); it != _ConfigServers.end(); ++it)
	{
		if (it->getPort() == port && (it->getHost() == mhost || it->getServerName() == mhost))
		{
			if (it->getRedirect().first != 0)
				return (*it);
			map<string, ConfigServer*> routes = it->getRoutes();
			for (map<string, ConfigServer*>::iterator it2 = routes.begin(); it2 != routes.end(); ++it2)
			{	
				if(!target.compare(0, it2->first.size(), it2->first) && it2->first.size() > match){
					ptr = it2->second;
				}
			}
			if (ptr)
				return (*ptr);
			// cout << "[getServerConfig]: target: \"" << target << "\" not found returning default settings for server." << endl;
			return (*it);
		}
	}
	throw (runtime_error("getServerConfig: Port/Host not found in " + _Filename));
}

void	Config::printConfig(){

	for (vector<ConfigServer>::iterator it = _ConfigServers.begin(); it != _ConfigServers.end(); ++it)
	{
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
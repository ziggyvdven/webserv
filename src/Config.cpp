#include "../includes/Config.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
Config::Config(string const & input) : _Linenumber(0), _NServers(0){
	regex 			config_filename(CONFIG_FOLDER + "\\/[a-zA-Z_0-9]+\\.conf");
	regex			server_reg("^server\\s*\\{");
	string			line;
	
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
		else if (!line.empty())
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
	_ConfigServers.push_back(ConfigServer(block, *this));
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

unordered_set<string>&		Config::getDirectives(){
	return (_Directives);
}

string						Config::getFilename() const{
	return (this->_Filename);
}

unsigned					Config::getLinenumber() const{
	return (_Linenumber);
}

void						Config::incrementLinenumber(){
	_Linenumber++;
}

vector<ConfigServer>&		Config::GetConfigServers(){
	return(_ConfigServers);
}

ConfigServer&				Config::GetServer(size_t index){
	if (index < 0 || index > _NServers)
		throw (runtime_error("GetServer index out of bounds"));
	return (_ConfigServers[index]);
}


/* ************************************************************************** */
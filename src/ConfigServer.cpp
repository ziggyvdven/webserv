#include "../includes/ConfigServer.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
ConfigServer::ConfigServer(Config & config): _Config(config), _Port(80), _Host("127.0.0.1"),
 _ServerName("default"), _ClientMaxBodySize(1048576), _AutoIndex(false), _Root("/www"), _Index("index.html"), _CGIbin("/cgi-bin"), _CGIext(".php"),
 _Return(0, ""), _Routes(), _SetDirectives(), _Target(""), _UploadDir("/uploads"){
	// std::cout << G << "ConfigServer constructor called" << END << std::endl;
	for (int i = 0; i < 3; i++)
		_Methods[i] = true;
}

void	ConfigServer::Init(vector<pair<string, unsigned> > const & conf){
	string 								whitespace = " \t";
	string								line;
	unordered_set<string>				directives = _Config.getDirectives();

	_Block = conf;
	for (vector<pair<string, unsigned> >::iterator it = _Block.begin(); it != _Block.end(); ++it){
		// cut the first word from the string
		istringstream iss(it->first);
		string line; 
		iss >> line;

		if (line == "location")
			it = CreateLocationBlocks(it);
		else if (directives.find(line) == directives.end()){
			throw (runtime_error("unknown directive \"" + line + "\" in " + _Config.getFilename() + ":" + to_string(it->second)));
		}
		else{
			Parseline(*it, line);
			_SetDirectives.push_back(it->first);
		}
	}
	if (!_Location_blocks.empty())
		CreateRoutes();
}

vector<pair<string, unsigned> >::iterator	ConfigServer::CreateLocationBlocks(vector<pair<string, unsigned> >::iterator it){
	vector<pair<string, unsigned> > 	block;
	pair<string, unsigned> 				new_pair;

	if (it->first.find("{") == string::npos)
		throw (runtime_error("Missing opening bracket in directive \"location\" in " + _Config.getFilename() + ":" + to_string(it->second)));
	while (it->first.find("}") == string::npos){
		istringstream iss(it->first);
		string line; 
		iss >> line;
		if (line == "listen" || line == "host" || line == "server_name")
			throw (runtime_error("\"" + line + "\" directive is not allowed here in " + _Config.getFilename() + ":" + to_string(it->second)));
		new_pair.first = it->first;
		new_pair.second = it->second;
		block.push_back(new_pair);
		++it;
	}
	_Location_blocks.push_back(block);
	return (it);
}

ConfigServer::ConfigServer( const ConfigServer & src )
	: _Config(src._Config)
{
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
		_Block = rhs._Block;
		_Config = rhs._Config;
		_Port = rhs._Port;
		_Host = rhs._Host;
		_ServerName = rhs._ServerName;
		_ClientMaxBodySize = rhs._ClientMaxBodySize;
		_ErrorPages = rhs._ErrorPages;
		_AutoIndex = rhs._AutoIndex;
		_Root = rhs._Root;
		for (int i = 0; i < 3; i++)
			_Methods[i] = rhs._Methods[i];
		_Index = rhs._Index;
		_CGIbin = rhs._CGIbin;
		_CGIext = rhs._CGIext;
		_Return = rhs._Return;
		_Routes = rhs._Routes;
		_Target = rhs._Target;
		_UploadDir = rhs._UploadDir;
	}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, ConfigServer const & i )
{
	o << "______________SERVER_CONFIG______________________________" << endl;
	o << "SEVER NAME| " << i.getServerName() << endl;
	o << "      HOST| " << i.getHost() << endl;
	o << "      PORT| " << i.getPort() << endl;
	o << "AUTO-INDEX| " << i.getAutoIndex() << endl;
	o << "      ROOT| " << i.getRoot() << endl;
	o << "     INDEX| " << i.getIndex() << endl;
	o << "   METHODS| " ;
	if (i.getMethod("GET"))
		o << "GET ";
	if (i.getMethod("POST"))
		o << "POST ";
	if (i.getMethod("DELETE"))
		o << "DELETE ";
	o << endl;
	if (i.getClientMaxBodySize() >= 1048576){
		long long max = i.getClientMaxBodySize() >> 20;
		o << "MAX C.BODY| " << max << " MegaBytes" << endl;
	}
	else
		o << "MAX C.BODY| " << i.getClientMaxBodySize() << " Bytes" << endl;
	o << "ERRORPAGES| ";
	for (map<short, string>::const_iterator it = i.getErrorPageMap().begin(); it != i.getErrorPageMap().end(); ++it) {
        o << it->first << ": " << it->second << " ";
    }
	o << endl;
	o << "   CGI_BIN| " << i.getCGIbin() << endl;
	o << "   CGI_EXT| " << i.getCGIext() << endl;
	o << "UPLOAD_DIR| " << i.getUploadDir() << endl;
	o << "  REDIRECT| " << i.getRedirect().first << " " << i.getRedirect().second << endl;
	o << "    TARGET| " << i.getTarget() << endl;

	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	ConfigServer::Parseline(pair<string, unsigned> &linepair, string& line){
    string	Directives[NUM_DIRECTIVES] = {
        "listen", "host", "server_name", "client_max_body_size", 
        "error_page", "autoindex", "root", "allow_methods",  
        "index", "cgi_bin", "cgi_ext", "return", "location", "upload_dir"
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
				case 5:
					ParseAutoIndex(linepair);
					break;
				case 6:
					ParseRoot(linepair);
					break;
				case 7:
					ParseMethods(linepair);
					break;
				case 8:
					ParseIndex(linepair);
					break;
				case 9:
					ParseCGIbin(linepair);
					break;
				case 10:
					ParseCGIext(linepair);
					break;
				case 11:
					ParseReturn(linepair);
					break;
				case 12:
					ParseReturn(linepair);
					break;
				case 13:
					ParseUploadDir(linepair);
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
	regex 	port_line("\\s*listen\\s*[0-9]+;\\s*");
	regex 	host_line("\\s*listen\\s*[0-9]+.[0-9]+.[0-9]+.[0-9]+:[0-9]+;\\s*");
	regex	localhost_line("\\s*listen\\s*localhost;\\s*");
	regex	port_localhost_line("\\s*listen\\s*localhost:[0-9]+;\\s*");
	string  line = linepair.first;

	if (find(_SetDirectives.begin(), _SetDirectives.end(), line) != _SetDirectives.end())
		throw (runtime_error("\"listen\" directive is duplicate in" + _Config.getFilename() + ":" + to_string(linepair.second)));
	if (regex_match(line, port_line)){
		size_t start = line.find_first_of("0123456789");
		size_t end = line.find_first_not_of("0123456789", start);
		line = line.substr(start, end - start);
		if (ValidatePort(line, to_string(linepair.second)))
			_Port = stoi(line);
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
	else if(regex_match(line, localhost_line) || regex_match(line, port_localhost_line)){
		size_t start = line.find_first_of(":") + 1;
		size_t end = line.find_first_of(";");
		line = line.substr(start, end - start);
		if (ValidatePort(line, to_string(linepair.second)))
			_Port = stoi(line);
		_Host = "127.0.0.1";
	}
	else 
		throw (runtime_error("Port or host not found in the \"listen\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseHost(pair<string, unsigned> & linepair){
	//Parses the host directive and uses the function inet_pton to check if the adressis valid. inet_pton returns
	regex 	host_line("\\s*host\\s*[0-9]+.[0-9]+.[0-9]+.[0-9]+;\\s*");
	regex	localhost_line("\\s*host\\s*localhost;\\s*");
	string  line = linepair.first;

	if (find(_SetDirectives.begin(), _SetDirectives.end(), line) != _SetDirectives.end())
		throw (runtime_error("\"host\" directive is duplicate in" + _Config.getFilename() + ":" + to_string(linepair.second)));
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
	else if(regex_match(line, localhost_line)){
		_Host = "127.0.0.1";
	}
	else 
		throw (runtime_error("Host not found in the \"host\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseServerName(pair<string, unsigned> & linepair){
	//Parses the servername only takes alphanumeric characters and upper and lower case
	regex 	servername_line("\\s*server_name\\s*[A-Za-z0-9-_.]+;\\s*");
	string  line = linepair.first;

	string servername = line.substr(line.find("server_name") + 12, line.find(";"));
	servername = trim(servername);
	servername.pop_back();

	if (regex_match(line, servername_line))
		_ServerName = servername;
	else 
		throw (runtime_error("Invalid server name \"" + servername + "\" in the \"server_name\" directive in /config/webserv.conf:" + to_string(linepair.second)));
}

void	ConfigServer::ParseClientMaxBodySize(pair<string, unsigned> & linepair){
	//Sets the maximum allowed size of the client request body. If the size in a request exceeds the configured value, the 413 (Request Entity Too Large) error is returned to the client. Setting size to 0 disables checking of client request body size.
	regex 	client_max_body_size_line("\\s*client_max_body_size\\s*[0-9]+[m]*;\\s*");
	string  line = linepair.first;
	size_t	clientmaxbodysize = 0;

	if (find(_SetDirectives.begin(), _SetDirectives.end(), line) != _SetDirectives.end())
		throw (runtime_error("\"client_max_body_size\" directive is duplicate in" + _Config.getFilename() + ":" + to_string(linepair.second)));
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
	// regex 	error_pages_line("\\s*error_page\\s+(\\d{3}\\s+)+[^\\s;]+;\\s*");
	string  line = linepair.first;

	//cut the error_page of the line;
	size_t start = line.find("error_page") + 11;
	size_t end = line.find(";");
	if (end && line.find_first_not_of(" \t", end + 1) != string::npos)
		throw (runtime_error("Missing trailing ';' in the \"error_page\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
	string errorpages = line.substr(line.find_first_not_of(" \t", start), end);

	//Cut the string in to tokens and checks if the error codes are valid
	istringstream iss(errorpages);
	vector<std::string> tokens;
	string token;
	while (iss >> token) 
		tokens.push_back(trim(token));
	string page = tokens.back();
	tokens.pop_back();
	page.pop_back();
	ParseErrorCodes(tokens, linepair.second);
	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
		_ErrorPages[stoi(*it)] = page;
}

void 	ConfigServer::ParseAutoIndex(pair<string, unsigned> & linepair){
	//Sets the maximum allowed size of the client request body. If the size in a request exceeds the configured value, the 413 (Request Entity Too Large) error is returned to the client. Setting size to 0 disables checking of client request body size.
	regex 	auto_index_line("\\s*autoindex\\s*(on|off);\\s*");
	string  line = linepair.first;

	if (find(_SetDirectives.begin(), _SetDirectives.end(), line) != _SetDirectives.end())
		throw (runtime_error("\"autoindex\" directive is duplicate in" + _Config.getFilename() + ":" + to_string(linepair.second)));
	
	size_t start = line.find("autoindex") + 9;
	start = line.find_first_not_of(" \t", start);
	size_t end = line.find(";");
	string autoindex = line.substr(start, end - start);
	if (regex_match(line, auto_index_line)){
		if 	(autoindex == "on")
			_AutoIndex = 1;
		if 	(autoindex == "off")
			_AutoIndex = 0;
	}
	else
		throw (runtime_error("Invalid value \"" + autoindex + "\" in the \"autoindex\" directive, it must be \"on\" or \"off\" in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseRoot(pair<string, unsigned> & linepair){
	//Sets the root folder if not set then the default folder will be /data
	regex 	root_line("\\s*root\\s*/([A-Za-z0-9-_.]+/)*[A-Za-z0-9-_.]+/*;\\s*");
	string  line = linepair.first;
	string root = line.substr(line.find("root") + 4, line.find(";"));

	if (find(_SetDirectives.begin(), _SetDirectives.end(), line) != _SetDirectives.end())
		throw (runtime_error("\"root\" directive is duplicate in" + _Config.getFilename() + ":" + to_string(linepair.second)));
	root = trim(root);
	root.pop_back();
	if (regex_match(line, root_line)){
		while (root.back() == '/')
			root.pop_back();
		_Root = root;
	}
	else
		throw (runtime_error("Invalid value \"" + root + "\" in the \"root\" directive, it must be \"/foo\", in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseMethods(pair<string, unsigned> & linepair){
	//Sets the allowed methods root folder if not set then the default folder will be /data
	// regex 	methods_line("\\s*allow_methods\\s*(GET|POST|DELETE|0)(?:\\s+(GET|POST|DELETE|0))*\\s*;\\s*");
	string  line = linepair.first;
	string methods = line.substr(line.find("allow_methods") + 13, line.find(";"));
	
	methods = trim(methods);
	methods.pop_back();
	if (methods.empty())
		throw (runtime_error("Invalid value \"" + methods + "\" in the \"allow_methods\" directive, it must be GET|POST|DELETE, in " + _Config.getFilename() + ":" + to_string(linepair.second)));
	for (int i = 0; i < 3; i++)
		_Methods[i] = false;
	istringstream iss(methods);
	string line2;
	while (iss >> line2)
	{
		if (line2 == "GET")
			_Methods[0] = true;
		else if (line2 == "POST")
			_Methods[1] = true;
		else if (line2 == "DELETE")
			_Methods[2] = true;
		else if (line2 == "0")
			continue;
		else
			throw (runtime_error("Invalid value \"" + line2 + "\" in the \"allow_methods\" directive, it must be GET|POST|DELETE, in " + _Config.getFilename() + ":" + to_string(linepair.second)));
	}
}

void	ConfigServer::ParseIndex(pair<string, unsigned> & linepair){
	//Sets the default file to answer if the request is a directory. By default it is set to index.html
	regex 	index_line("\\s*index\\s*[A-Za-z0-9-_.]+\\s*;\\s*");
	string  line = linepair.first;

	string index = line.substr(line.find("index") + 5, line.find(";"));
	index = trim(index);
	index.pop_back();

	if (regex_match(line, index_line)){
		_Index = index;
	}
	else
		throw (runtime_error("Invalid value \"" + index + "\" in the \"index\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseCGIbin(pair<string, unsigned> & linepair){
	//Sets the root folder if not set then the default folder will be /data
	regex 	cgi_bin_line("\\s*cgi_bin\\s*/[A-Za-z0-9-_./]+\\s*;\\s*");
	string  line = linepair.first;

	string cgi_bin = line.substr(line.find("cgi_bin") + 7, line.find(";"));
	cgi_bin = trim(cgi_bin);
	cgi_bin.pop_back();

	if (regex_match(line, cgi_bin_line)){
		_CGIbin = cgi_bin;
	}
	else
		throw (runtime_error("Invalid value \"" + cgi_bin + "\" in the \"cgi_bin\" directive, must be \"/PATH_INFO\" in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseCGIext(pair<string, unsigned> & linepair){
	//Sets the root folder if not set then the default folder will be /data
	regex 	cgi_ext_line("\\s*cgi_ext\\s*\\.[A-Za-z0-9]+\\s*;\\s*");
	string  line = linepair.first;

	string cgi_ext = line.substr(line.find("cgi_ext") + 7, line.find(";"));
	cgi_ext = trim(cgi_ext);
	cgi_ext.pop_back();

	if (regex_match(line, cgi_ext_line)){
		_CGIext = cgi_ext;
	}
	else
		throw (runtime_error("Invalid value \"" + cgi_ext + "\" in the \"cgi_ext\" directive, must be \".FOO\" in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseUploadDir(pair<string, unsigned> & linepair){
	//Sets the root folder if not set then the default folder will be /data
	regex 	upload_dir_line("\\s*upload_dir\\s*/[A-Za-z0-9-_./]+\\s*;\\s*");
	string  line = linepair.first;

	string upload_dir = line.substr(line.find("upload_dir") + 10, line.find(";"));
	upload_dir = trim(upload_dir);
	upload_dir.pop_back();

	if (regex_match(line, upload_dir_line)){
		_UploadDir = upload_dir;
	}
	else
		throw (runtime_error("Invalid value \"" + upload_dir + "\" in the \"upload_dir\" directive, must be \".FOO\" in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::ParseReturn(pair<string, unsigned> & linepair){
	//Sets the root folder if not set then the default folder will be /data
	regex 	return_line("\\s*return\\s+\\d{3}\\s*[^\\s;]*\\s*;\\s*");
	string  line = linepair.first;

	string redirect = line.substr(line.find("return") + 6, line.find(";"));
	redirect = trim(redirect);
	redirect.pop_back();
	if (regex_match(line, return_line)){
		istringstream iss(redirect);
		iss >> _Return.first;
		iss >> _Return.second;
		if (_Return.first < 0 || _Return.first > 999)
			throw (runtime_error("Invalid value \"" + to_string(_Return.first) + "\" in the \"return\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
	}
	else
		throw (runtime_error("Invalid value \"" + redirect + "\" in the \"return\" directive in " + _Config.getFilename() + ":" + to_string(linepair.second)));
}

void	ConfigServer::CreateRoutes(){
	
	for(vector<vector<pair<string, unsigned> > >::iterator it = _Location_blocks.begin(); it != _Location_blocks.end(); ++it){
		string line;
		istringstream iss(it->begin()->first);
		iss >> line;
		iss >> line;
		it->erase(it->begin());
		ConfigServer* location = new ConfigServer(*this);
		location->Init(*it);
		location->setTarget(line);
		_Routes[location->getTarget()] = location;
	}
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

Config&	ConfigServer::getConfig(){
	return (this->_Config);
}

unsigned short	ConfigServer::getPort() const{
	return (this->_Port);
}

string	ConfigServer::getHost() const{
	return (this->_Host);
}

string	ConfigServer::getServerName() const{
	return (this->_ServerName);
}

size_t	ConfigServer::getClientMaxBodySize() const{
	return (this->_ClientMaxBodySize);
}

string	ConfigServer::getErrorPage(short const & errorcode) const{
	try {
		string errorpage = _ErrorPages.at(errorcode);
		return errorpage;
	}
	catch (const std::out_of_range& e) {
    }
	return ("");
}

const map<short, string>&	ConfigServer::getErrorPageMap() const{
	return (this->_ErrorPages);
}

bool	ConfigServer::getAutoIndex() const{
	return (this->_AutoIndex);
}

string	ConfigServer::getRoot() const{
	return (this->_Root);
}

string	ConfigServer::getIndex() const{
	return (this->_Index);
}

bool	ConfigServer::getMethod(string method) const{
	if (method == "GET" && _Methods[0])
		return true;
	if (method == "POST" && _Methods[1])
		return true;
	if (method == "DELETE" && _Methods[2])
		return true;
	return false;
}

string	ConfigServer::getCGIbin() const{
	return (this->_CGIbin);
}

string	ConfigServer::getCGIext() const{
	return (this->_CGIext);
}

string	ConfigServer::getUploadDir() const{
	return (this->_UploadDir);
}

pair<short, string>	ConfigServer::getRedirect() const{
	return (this->_Return);
}

map<string, ConfigServer*>	ConfigServer::getRoutes() const{
	return (this->_Routes);
}

void	ConfigServer::setTarget(string const & route){
	_Target = route;
}

string	ConfigServer::getTarget() const{
	return (this->_Target);
}

/* ************************************************************************** */
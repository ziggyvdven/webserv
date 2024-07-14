#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "Common.hpp"
# include "Config.hpp"
# include <vector>
# include <string>
# include <map>
# include <unordered_set>
# include <arpa/inet.h>
# include <sstream>
# include <algorithm>

using namespace std;

# define BOLD "\033[1;37m"
# define END "\033[0m"
# define G "\033[0;32m"
# define B "\033[0;34m"
# define R "\033[0;31m"

class Config;

class ConfigServer {
public:
	ConfigServer(Config & config);
	ConfigServer ( ConfigServer const & src);
	~ConfigServer( void );

	ConfigServer &		operator=( ConfigServer const & rhs );
	void				Init(vector<pair<string, unsigned> > const & conf);


	unsigned short				getPort() const;
	string						getHost() const;
	string						getServerName() const;
	long long					getClientMaxBodySize() const;
	string						getErrorPage(short const & errorcode) const;
	const map<short, string>&	getErrorPageMap() const;
	bool						getAutoIndex() const;
	string						getRoot() const;
	string						getIndex() const;
	bool						getMethod(string method) const;
	string						getCGIbin() const;
	string						getCGIext() const;
	pair<short, string>			getRedirect() const;
	map<string, ConfigServer*>	getRoutes() const;

	void						setHost(string const & route);

private:
	vector<pair<string, unsigned> > 	_Block;
	Config&								_Config;
	unsigned short						_Port;
	string								_Host;
	string								_ServerName;
	long long							_ClientMaxBodySize; // in bytes
	map<short, string>					_ErrorPages;
	bool								_AutoIndex;
	string								_Root;
	bool								_Methods[3]; // GET[0], POST[1], and DELETE[2]
	string								_Index;
	string								_CGIbin;
	string								_CGIext;
	pair<short, string>					_Return;
	map<string, ConfigServer*>			_Routes;
	vector<vector<pair<string, unsigned> > > _Location_blocks;

	bool				ValidatePort(string& line, string N) const;
	string 				trim(const std::string& str);
	void				CreateRoutes( void );

	void				Parseline(pair<string, unsigned> & linepair, string& line);
	void				ParseListen(pair<string, unsigned> & linepair);
	void				ParseHost(pair<string, unsigned> & linepair);
	void				ParseServerName(pair<string, unsigned> & linepair);
	void				ParseClientMaxBodySize(pair<string, unsigned> & linepair);
	void				ParseErrorPage(pair<string, unsigned> & linepair);
	void				ParseErrorCodes(std::vector<std::string> &tokens, unsigned const & linenumber) const;
	void 				ParseAutoIndex(pair<string, unsigned> & linepair);
	void				ParseRoot(pair<string, unsigned> & linepair);
	void				ParseMethods(pair<string, unsigned> & linepair);
	void				ParseIndex(pair<string, unsigned> & linepair);
	void				ParseCGIbin(pair<string, unsigned> & linepair);
	void				ParseCGIext(pair<string, unsigned> & linepair);
	void				ParseReturn(pair<string, unsigned> & linepair);

};

std::ostream &			operator<<( std::ostream & o, ConfigServer const & i );

#endif /* ********************************************************** SERVER_H */
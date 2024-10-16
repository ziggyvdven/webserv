#include "../includes/CgiHandler.hpp"
#include "../includes/utils.hpp"
#include <strings.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <iostream>	
#include <fstream>
#include <vector>
#include <ctime>
#include <signal.h>
#include <sys/wait.h>

#define TIMEOUT 12

CgiHandler::CgiHandler(HttpRequest const &request, ConfigServer* config)
	: _request(request), _htmlRoot("./data/www"), _ConfigServer(config)
{
	if (!request.isValid())
		throw std::exception();
}

bool CgiHandler::isCgiRequest() const
{
	return _is_valid; 
}

bool	CgiHandler::isCgiScript(std::string const &target)
{
	std::string	cgi_bin = _ConfigServer->getCGIbin();

	if (cgi_bin.back() != '/')
		cgi_bin += "/";
	size_t				it = target.find(cgi_bin);
	
	_scriptName = "SCRIPT_NAME=";
	_pathInfo = "PATH_INFO=";
	_queryString = "QUERY_STRING=";

	if (it == 0)
	{
		it = target.find_first_of("/?", cgi_bin.size());
		std::string const	script_name = target.substr(0, it);
		std::string const	script_path = _htmlRoot + script_name;
		_scriptPath = _htmlRoot + script_name;
		if (access(script_path.data(), X_OK) == 0)
		{
			_scriptName += script_name;
			if (target[it] == '/')
			{
				size_t	it_query = target.find_first_of('?', it);
				_pathInfo += target.substr(it, it_query - it);
				it = it_query;
			}
			if (target[it] == '?')
			{
				_queryString += target.substr(it + 1);
			}
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: %s", _ConfigServer->getServerName().c_str(), _scriptName.c_str());
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: %s", _ConfigServer->getServerName().c_str(), _pathInfo.c_str());
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: %s", _ConfigServer->getServerName().c_str(), _queryString.c_str());
			_envp.push_back(_scriptName.data());
			_envp.push_back(_pathInfo.data());
			_envp.push_back(_queryString.data());
			return (true);
		}
	}
	return (false);
}

std::string	CgiHandler::execCgiScript()
{
	std::vector<char const *>	argv;
	pid_t						process_id;

	std::string	const			root = "ROOT=" + _htmlRoot;
	std::string	const			version = "HTTP_VERSION=" + _request.version();
	std::string	const			method = "REQUEST_METHOD=" + _request.method();
	std::string const			filename = "FILENAME=./data/www/upload/test.txt";
	std::string const			content_type = "CONTENT_TYPE=" + _request.getHeader("content-type");
	std::string const			upload_folder = "UPLOAD_DIR=" + _htmlRoot + _ConfigServer->getUploadDir(); 

	std::stringstream			content_length;
	content_length << "CONTENT_LENGTH=" << _request.body().size();
	
	int							child_to_parent[2], parent_to_child[2];
	int							wstatus;

	pipe(child_to_parent);
	pipe(parent_to_child);

	process_id = fork();
	if (process_id < 0)
		perror ("fork() failed");
	else if (process_id == 0)
	{
		// _webServer.cleanUpSockets();
		close(parent_to_child[1]);
		dup2(parent_to_child[0], STDIN_FILENO);
		close(parent_to_child[0]);
		
		close(child_to_parent[0]);
		dup2(child_to_parent[1], STDOUT_FILENO);
		close(child_to_parent[1]);
		
		argv.push_back(_scriptPath.data());
		argv.push_back(NULL);

		_envp.push_back(root.data());
		_envp.push_back(version.data());
		_envp.push_back(method.data());
		_envp.push_back(filename.data());
		_envp.push_back(content_length.str().data());
		_envp.push_back(content_type.data());
		_envp.push_back(upload_folder.data());
		_envp.push_back(NULL);

		execve (_scriptPath.data(), const_cast<char * const *>(argv.data()), const_cast<char * const *>(_envp.data()));
		exit (127);
	}

	close(parent_to_child[0]);
	close(child_to_parent[1]);

	// Send request body to CGI
	
	// TODO: c'est un peu de la chnoute
	unsigned long chunk_size = 500000;
	for (unsigned long i = 0; i < _request.body().size(); i += chunk_size)
	{
		chunk_size = ::min(chunk_size, _request.body().size() - i);
		write(parent_to_child[1], _request.body().data() + i, chunk_size);
	}
	close(parent_to_child[1]);

	if (_timeout_cgi(process_id, wstatus, TIMEOUT))
	{
		kill(process_id, 9);
		_cgiResponse = "<h1>[CGI] Script timed out!</h1>";
		return _cgiResponse;
	}

	if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
	{
		// DEBUG
		_ConfigServer->getConfig().printMsg(B, "Server[%s]: [CGI] Process exited with error code", _ConfigServer->getServerName().c_str());
		std::cout << std::boolalpha << B << "Exited: " << WIFEXITED(wstatus)\
			<< "\nExit status: " << WEXITSTATUS(wstatus)\
			<< "\nSignaled: " << WIFSIGNALED(wstatus)\
			<< END << std::endl;

		_cgiResponse = "<h1>[DEBUG] Error in executing CGI script</h1>\r\n";
	}

	#define BUFFERSIZE 255
	char buffer[BUFFERSIZE];
	bzero(buffer, BUFFERSIZE);
	
	int bytes_read;
	while((bytes_read = read (child_to_parent[0], buffer, BUFFERSIZE)) > 0)
	{
		buffer[bytes_read] = '\0';
		_cgiResponse += buffer;
	}

	return (_cgiResponse);
}

bool CgiHandler::_timeout_cgi(int process_id, int &wstatus, int timeout_sec)
{
	std::chrono::steady_clock::time_point begin;
	begin = std::chrono::steady_clock::now();

	_ConfigServer->getConfig().printMsg(B, "Server[%s]: [Timing for ] %d", _ConfigServer->getServerName().c_str(), timeout_sec);
	while (true)
	{
		waitpid(process_id, &wstatus, WNOHANG);
		if (WIFEXITED(wstatus))
		{
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: Child process finished", _ConfigServer->getServerName().c_str());
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: Elapsed time: %d", _ConfigServer->getServerName().c_str(), time_since(begin));
			return false;
		}

		if (time_since(begin) > timeout_sec * 1000)
		{
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: Cgi-handler [TIMEOUT]", _ConfigServer->getServerName().c_str());
			return true;
		}
		usleep(50000);
	}
	return false;
}

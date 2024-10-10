#include "../includes/CgiHandler.hpp"
#include "../includes/utils.hpp"
#include <strings.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <signal.h>
#include <sys/wait.h>

#define TIMEOUT 12

CgiHandler::CgiHandler(HttpRequest const &request, ConfigServer* config)
	: _request(request), _htmlRoot("./data/www"), _ConfigServer(config)
{
	if (request.hasError())
		throw std::exception();

	_env_strings.reserve(32);
	_envp.reserve(32);
	_is_valid = false;
	_state = COLD;

	_init();
}

void CgiHandler::_init() {

	std::string	cgi_bin = _ConfigServer->getCGIbin();
	const std::string target = _request.target();

	if (cgi_bin.back() != '/')
		cgi_bin += "/";
	size_t	it = target.find(cgi_bin);

	if (it != 0) {
		return;
	}

	it = target.find_first_of("/?", cgi_bin.size());
	_scriptName = target.substr(0, it);
	// if (target.find(".bla") != std::string::npos)
	// 	script_name = "/cgi-bin/cgi_tester";
	_scriptPath = _htmlRoot + _scriptName;

	if (access(_scriptPath.data(), X_OK) != 0) {
		return ;
	}

	if (target[it] == '/') {
		size_t	it_query = target.find_first_of('?', it);
		_pathInfo = target.substr(it, it_query - it);
		it = it_query;
	}

	if (target[it] == '?') {
		_queryString = target.substr(it + 1);
	}

	_ConfigServer->getConfig().printMsg(B, "Server[%s]: %s", _ConfigServer->getServerName().c_str(), _scriptName.c_str());
	_ConfigServer->getConfig().printMsg(B, "Server[%s]: %s", _ConfigServer->getServerName().c_str(), _pathInfo.c_str());
	_ConfigServer->getConfig().printMsg(B, "Server[%s]: %s", _ConfigServer->getServerName().c_str(), _queryString.c_str());

	this->_is_valid = true;
	_setEnvp();
}

void CgiHandler::_setEnvp() {
	_add_env_var("SCRIPT_NAME", _scriptName);
	_add_env_var("PATH_INFO", _pathInfo);
	_add_env_var("QUERY_STRING", _queryString);
	_add_env_var("ROOT", _htmlRoot);
	_add_env_var("HTTP_VERSION", _request.version());
	_add_env_var("REQUEST_METHOD", _request.method());
	_add_env_var("FILENAME", "/data/www/upload/test.txt");
	_add_env_var("CONTENT_TYPE", _request.getHeader("content-type"));
	_add_env_var("UPLOAD_DIR", _htmlRoot + _ConfigServer->getUploadDir());
	_envp.push_back(NULL);

}
void CgiHandler::_add_env_var(std::string key, std::string value) {
	_env_strings.push_back(key + "=" + value);
	_envp.push_back(_env_strings.back().c_str());
}

bool CgiHandler::isValid() const
{
	return _is_valid;
}

bool CgiHandler::_spawn_process() {
	std::vector<char const *>	argv;

	std::stringstream			content_length;
	content_length << "CONTENT_LENGTH=" << _request.body().size();


	pipe(_child_to_parent);
	pipe(_parent_to_child);

	_process_id = fork();
	if (_process_id < 0)
		perror ("fork() failed");
	else if (_process_id == 0)
	{
		// _webServer.cleanUpSockets();
		close(_parent_to_child[1]);
		dup2(_parent_to_child[0], STDIN_FILENO);
		close(_parent_to_child[0]);

		close(_child_to_parent[0]);
		dup2(_child_to_parent[1], STDOUT_FILENO);
		close(_child_to_parent[1]);

		argv.push_back(_scriptPath.data());
		argv.push_back(NULL);

		execve (_scriptPath.data(), const_cast<char * const *>(argv.data()), const_cast<char * const *>(_envp.data()));
		exit (127);
	}

	close(_parent_to_child[0]);
	close(_child_to_parent[1]);
	return true;
}

void	CgiHandler::run()
{
	int							wstatus;

	if (_state == COLD) {
		_spawn_process();
		if (_request.getContentLength() > 0) {
			_state = SENDING_TO_SCRIPT;
		} else {
			_state = READING_FROM_SCRIPT;
		}
	}

	// Check time-out
	if (_timeout_cgi(_process_id, wstatus, TIMEOUT))
	{
		kill(_process_id, 9);
		_cgiResponse = "<h1>[CGI] Script timed out!</h1>";
		_state = TIMED_OUT;
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

	if (_state == SENDING_TO_SCRIPT)
	{
		// write chunk
		unsigned long chunk_size = 500000;
		for (unsigned long i = 0; i < _request.body().size(); i += chunk_size)
		{
			chunk_size = ::min(chunk_size, _request.body().size() - i);
			write(_parent_to_child[1], _request.body().data() + i, chunk_size);
		}
		close(_parent_to_child[1]);
		_state = READING_FROM_SCRIPT;
	}

	if (_state == READING_FROM_SCRIPT)
	{
		// Read chunk
		#define BUFFERSIZE 255
		char buffer[BUFFERSIZE];
		bzero(buffer, BUFFERSIZE);
		int bytes_read;

		while((bytes_read = read (_child_to_parent[0], buffer, BUFFERSIZE)) > 0)
		{
			buffer[bytes_read] = '\0';
			_cgiResponse += buffer;
		}

		_state = COMPLETE;
	}
	// If the handler is marked as complete, web client reads the _cgiResponse, and sends it
	// TODO [Optional]: make the webclient send data as it comes, with chunked transfer.
}

bool CgiHandler::_timeout_cgi(int _process_id, int &wstatus, int timeout_sec)
{
	std::time_t begin = std::time(NULL);

	_ConfigServer->getConfig().printMsg(B, "Server[%s]: [Timing for ] %d", _ConfigServer->getServerName().c_str(), timeout_sec);
	while (true)
	{
		waitpid(_process_id, &wstatus, WNOHANG);
		if (WIFEXITED(wstatus))
		{
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: Child process finished", _ConfigServer->getServerName().c_str());
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: Elapsed time: %d", _ConfigServer->getServerName().c_str(), seconds_since(begin));
			return false;
		}

		if (seconds_since(begin) > timeout_sec * 1000)
		{
			_ConfigServer->getConfig().printMsg(B, "Server[%s]: Cgi-handler [TIMEOUT]", _ConfigServer->getServerName().c_str());
			return true;
		}
		usleep(50000);
	}
	return false;
}

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

CgiHandler::CgiHandler(HttpRequest const &request, std::string const &cgi_bin)
	: _request(request), _htmlRoot("./data/www"), _cgi_bin(cgi_bin)
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

	const std::string target = _request.target();

	if (_cgi_bin.back() != '/')
		_cgi_bin += "/";
	size_t	it = target.find(_cgi_bin);

	if (it != 0) {
		return;
	}

	it = target.find_first_of("/?", _cgi_bin.size());
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

	// printMsg(B, "CGI: %s _scriptName", _scriptName.c_str());
	// printMsg(B, "CGI: %s _pathInfo", _pathInfo.c_str());
	// printMsg(B, "CGI: %s _queryString", _queryString.c_str());

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

bool CgiHandler::completed() const {
	return (_state == COMPLETE || _state == TIMED_OUT);
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

		std::cerr 
			<< " [CGI] scriptPath" << _scriptPath << "\n"
			<< " [CGI] argv: " << const_cast<char * const *>(argv.data()) << "\n"
			<< std::endl; 

		execve (_scriptPath.data(), const_cast<char * const *>(argv.data()), const_cast<char * const *>(_envp.data()));
		exit (127);
	}

	exit(0);

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
	// if (_timeout_cgi(_process_id, wstatus, TIMEOUT))
	// {
	// 	kill(_process_id, 9);
	// 	_cgiResponse = "<h1>[CGI] Script timed out!</h1>";
	// 	_state = COMPLETE;
	// 	return;
	// }

	if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
	{
		// DEBUG
		printMsg(B, "CGI: [CGI] Process exited with error code");
		std::cout << std::boolalpha << B << "Exited: " << WIFEXITED(wstatus)\
			<< "\nExit status: " << WEXITSTATUS(wstatus)\
			<< "\nSignaled: " << WIFSIGNALED(wstatus)\
			<< END << std::endl;

		_cgiResponse = "<h1>[DEBUG] Error in executing CGI script</h1>\r\n";
		_state = COMPLETE;
		return;
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
		return;
	}

	if (_state == READING_FROM_SCRIPT)
	{
		// Read chunk
		#define BUFFERSIZE 255
		char buffer[BUFFERSIZE];
		bzero(buffer, BUFFERSIZE);
		int bytes_read;

		bytes_read = read (_child_to_parent[0], buffer, BUFFERSIZE);
		printMsg(G, "CGI: bytes_read(%i) buffer: %s", bytes_read, buffer);
		if (bytes_read > 0)
		{
			buffer[bytes_read] = '\0';
			_cgiResponse += buffer;
		}

		if (bytes_read == 0 && _cgiResponse.size() > 0)
			_state = COMPLETE;
	}
	// If the handler is marked as complete, web client reads the _cgiResponse, and sends it
	// TODO [Optional]: make the webclient send data as it comes, with chunked transfer.
}

bool CgiHandler::_timeout_cgi(int _process_id, int &wstatus, int timeout_sec)
{
	std::time_t begin = std::time(NULL);

	printMsg(B, "CGI: [Timing for ] %d", timeout_sec);
	while (true)
	{
		waitpid(_process_id, &wstatus, WNOHANG);
		if (WIFEXITED(wstatus))
		{
			printMsg(B, "CGI: Child process finished");
			printMsg(B, "CGI: Elapsed time: %d", seconds_since(begin));
			return false;
		}

		if (seconds_since(begin) > timeout_sec * 1000)
		{
			printMsg(B, "CGI: Cgi-handler [TIMEOUT]");
			return true;
		}
		usleep(50000);
	}
	return false;
}

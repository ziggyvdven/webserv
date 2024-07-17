#include "../includes/CgiHandler.hpp"

CgiHandler::CgiHandler(HttpRequest const &request)
	: _request(request), _htmlRoot("./data/www")
{
	if (!request.isValid())
		throw std::exception();

	// Check if target is cgi
		// set _is_valid accordingly
		// stop if invalid
		_is_valid = true;

}

bool CgiHandler::isCgiRequest() const
{
	return _is_valid; 
}


std::string CgiHandler::execCgi()
{
	if(!isCgiRequest())
	{
		throw std::exception();
	}

	// send post data to stdin
	// execve the script

	// timeout

	return "<Cgi Response>";
}

bool	CgiHandler::isCgiScript(std::string const &target)
{
	std::string const	cgi_bin = "/cgi-bin/";
	size_t				it = target.find(cgi_bin);
	
	_scriptName = "SCRIPT_NAME=";
	_pathInfo = "PATH_INFO=";
	_queryString = "QUERY_STRING=";

	if (it == 0)
	{
		it = target.find_first_of("/?", cgi_bin.size());
		std::string const	script_name = target.substr(0, it);
		std::string const	script_path = _htmlRoot + script_name;
		if (access(script_path.c_str(), X_OK) == 0)
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
			std::cout << _scriptName << std::endl;
			std::cout << _pathInfo << std::endl;
			std::cout << _queryString << std::endl;
			return (true);
		}
	}
	return (false);
}

bool	CgiHandler::execCgiScript()
{
	std::string const			script_path = _htmlRoot + "/cgi-bin/upload.py";
	std::vector<char const *>	argv;
	std::vector<char const *>	envp;
	pid_t						process_id;

	std::string	const			version = "HTTP_VERSION=" + _request.version();
	std::string	const			method = "METHOD=" + _request.method();
	std::string const			filename = "FILENAME=./data/www/upload/test.txt";
	std::string	const			content = "This is a test";

	std::stringstream			content_length;
	content_length << "CONTENT_LENGTH=" << _request.body().size();
	
	int							wstatus;
	int							pipe_fd[2];

	pipe(pipe_fd);

	process_id = fork();
	if (process_id < 0)
		perror ("fork() failed");
	else if (process_id == 0)
	{
		// _webServer.cleanUpSockets();

		dup2 (pipe_fd[0], STDIN_FILENO);
		dup2 (pipe_fd[1], STDOUT_FILENO);

		close (pipe_fd[0]);
		close (pipe_fd[1]);
		
		std::cout << _request.body() << std::endl;

		argv.push_back(script_path.c_str());
		argv.push_back(NULL);

		envp.push_back(version.c_str());
		envp.push_back(method.c_str());
		envp.push_back(filename.c_str());
		envp.push_back(content_length.str().c_str());
		envp.push_back(NULL);

		execve (script_path.c_str(), const_cast<char * const *>(argv.data()), const_cast<char * const *>(envp.data()));
		exit (EXIT_FAILURE);
	}
	waitpid (process_id, &wstatus, 0);
	if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 0)
	{
		return (false);
	}
	// read (pipe_fd[0], buffer, 255);
	close (pipe_fd[0]);
	close (pipe_fd[1]);
	return (true);
}

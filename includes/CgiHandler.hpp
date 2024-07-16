#include "../includes/HttpRequest.hpp"

class CgiHandler
{
private:
	bool _is_valid;

public:
	CgiHandler(HttpRequest request);
	std::string execCgi();

	bool isCgiRequest() const; 

};

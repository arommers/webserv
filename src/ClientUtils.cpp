#include "../includes/Client.hpp"

void	Client::isValidMethod( std::string method )
{
	std::vector<std::string> validMethods = {"POST", "GET", "DELETE"};

	if (method.empty())
		setStatusCode(400);
	else if (std::find(validMethods.begin(), validMethods.end(), method) == validMethods.end())
		setStatusCode(405);
}

void	Client::isValidPath( std::string path )
{
	if (path.empty())
		setStatusCode(400);
}

void	Client::isValidVersion( std::string version )
{
	std::regex versionRegex(R"(HTTP\/\d\.\d)");

	if (version.empty())
		setStatusCode(400);
	else if (!std::regex_match(version, versionRegex))
		setStatusCode(505);
}
#include <../includes/Parsing.hpp>
#include <../includes/Client.hpp>


void	Parsing::isValidMethod( std::string method, Client& client )
{
	std::vector<std::string> validMethods = {"POST", "GET", "DELETE"};

	if (method.empty())
	    client.setStatusCode(400);
	else if (std::find(validMethods.begin(), validMethods.end(), method) == validMethods.end())
		client.setStatusCode(405);
}

void	Parsing::isValidPath( std::string path, Client& client )
{
	if (path.empty())
		client.setStatusCode(400);
}

void	Parsing::isValidVersion( std::string version, Client& client )
{
	std::regex versionRegex(R"(HTTP\/\d\.\d)");

	if (version.empty())
		client.setStatusCode(400);
	else if (!std::regex_match(version, versionRegex))
		client.setStatusCode(505);
}
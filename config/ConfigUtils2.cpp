#include "ServerInfo.hpp"
#include "Location.hpp"
#include "Config.hpp"

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Helper functions ---

// // Helper function to check if a string is a valid number
static bool	isValidNumber(const std::string &str)
{
	for (char c : str)
	{
		if (!std::isdigit(c))
			return false;
	}
	return true;
}
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Checkers functions ---
void	Config::ft_checkHost(const std::string &newHost, ServerInfo &server)
{
	// Check if variable is already set
	if (!server.getHost().empty())
		throw  Exception_Config("Host is duplicated");

	// Check if the value is matches
	if (newHost == "localhost" || newHost == "127.0.0.1")
		return ;
	else
		throw  Exception_Config("Invald Host");
}

/* NOTE:
 * Why range from 0 to 65535? 
 * - Port numbers can be up to five digits long
 * - Well-Known Ports (0 to 1023)
 * - Registered Ports (1024 to 49151)
 * - Dynamic or Private Ports (49152 to 65535)
 */
void	Config::ft_checkPort(const std::string &newPort, ServerInfo &server)
{
	// Check if variable is already set
	if (server.getPort() != 0)
		throw  Exception_Config("Port is duplicated");

	// Check if the value is a number
	if (!isValidNumber(newPort))
		throw Exception_Config("Invalid Port");

	// Convert string to an integer
	int maxClient = 0;
	try {
		maxClient = std::stoi(newPort);
	} catch (const std::invalid_argument&) {
		throw Exception_Config("Invalid Port: not a number");
	} catch (const std::out_of_range&) {
		throw Exception_Config("Invalid Port: out of range");
	}

	// Check if the value is within the valid range
	if (maxClient < 1 || maxClient > 65535)
		throw Exception_Config("Invalid Port: must be between 1 and 65535");
}

std::string	Config::ft_checkRoot(const std::string &newRoot, ServerInfo &server)
{
	std::string NewFullRoot;

	// Check if variable is already set
	if (!server.getRoot().empty())
		throw  Exception_Config("Root is duplicated");

	// Check if the first char is a '/'
	if (newRoot[0] != '/')
		throw Exception_Config("Invalid Root: Wrong syntax");

	// If the path type is a 2 directory
	if (Config::getPathType(newRoot) == FOLDER)
		return newRoot;
	else
	{
		// Concatenates the current working directory (dir) with the given NewRoot path, resulting in a full path
		char dir[1024];
		getcwd(dir, 1024);
		NewFullRoot = dir + newRoot;
		if (Config::getPathType(NewFullRoot) != FOLDER) // If the path type is not a directory
			throw Exception_Config("Invalid Root");
	}
	return NewFullRoot;
}

/* NOTE: 
 * MaxClient = File upload limit
 * Our limit is 5MB -> 5000000 bytes
 */
void	Config::ft_checkMaxClient(const std::string &newMaxClient, ServerInfo &server)
{
	// Check if variable is already set
	if (server.getMaxClient() != 0)
		throw  Exception_Config("max_client_size is duplicated");

	// Check if the value is a number
	if (!isValidNumber(newMaxClient))
		throw Exception_Config("Invalid max_client_size");

	// Convert string to an integer
	int maxClient = 0;
	try {
		maxClient = std::stoi(newMaxClient);
	} catch (const std::invalid_argument&) {
		throw Exception_Config("Invalid max_client_size: not a number");
	} catch (const std::out_of_range&) {
		throw Exception_Config("Invalid max_client_size: out of range");
	}

	// Check if the value is within the valid range
	if (maxClient < 1 || maxClient > 5000000)
		throw Exception_Config("Invalid max_client_size: must be between 1 and 5000000");
}

void	Config::ft_checkIndex(const std::string &newIndex, ServerInfo &server)
{
	// Check if variable is already set
	if (!server.getIndex().empty())
		throw  Exception_Config("Index is duplicated");

	// Check if the first char is a '/'
	if (newIndex[0] != '/')
		throw Exception_Config("Invalid Index: Wrong syntax");

	// Check if the last 5 characters are '.html'
	if (newIndex.size() < 5 || newIndex.substr(newIndex.size() - 5) != ".html")
		throw Exception_Config("Invalid Index: check your '.html' file");
}

void	Config::ft_checkServerName(const std::string &newServerName, ServerInfo &server)
{
	// Check if variable is already set
	if (!server.getServerName().empty())
		throw  Exception_Config("Server_name is duplicated");

	// Check if the value is valid (characters & number)
	for (char c : newServerName)
	{
		if (!std::isalnum(c))
			throw  Exception_Config("Invalid Server name");
	}
}

void	Config::ft_checkLocation(const std::vector<Location> &newLocation, ServerInfo &server)
{
	// Check if variable is already set
}

void	Config::ft_checkErrorPage(const std::vector<std::string> &newErrorPage, ServerInfo &server)
{
	// Check if variable is already set
}
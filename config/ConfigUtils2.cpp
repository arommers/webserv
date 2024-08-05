#include "ServerInfo.hpp"
#include "Location.hpp"
#include "Config.hpp"

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Helper functions ---

// Static helper function to check if a string is a valid number
static bool	isValidNumber(const std::string &str)
{
	for (char c : str)
	{
		if (!std::isdigit(c))
			return false;
	}
	return true;
}

// Static helper function to trim whitespace from a string
std::string	trim(const std::string &str)
{
	std::string result = str;
	result.erase(0, result.find_first_not_of(" \t"));
	result.erase(result.find_last_not_of(" \t") + 1);
	return result;
}

// Helper function that splits the loctaion blocks into 'key' and 'value'
std::vector<std::vector<std::string>> Config::ft_splitLocationParameters(const std::string newLocation0, const std::string &newLocation1)
{
	std::vector<std::vector<std::string>> parameters(2);
	std::vector<std::string> &keys = parameters[0];
	std::vector<std::string> &values = parameters[1];
	std::string key;
	std::string value;

	// -- newLocation0 --
	// getting the 'key' and 'value' from newLocation0
	// newLocation0 = 'location /root {' -> first line of a location Block
	key = newLocation0.substr(0, 8);
    value = newLocation0.substr(8, newLocation0.size() - 9);

	// Remove whitespace from 'key' and 'value'
	key = trim(key);
	value = trim(value);

	// Push on the vector of newLocation0
	keys.push_back(key);
	values.push_back(value);

	// -- newLocation1 --
	std::vector<std::string> lines = ft_splitStringByNewline(newLocation1);
	for (size_t i = 0; i < lines.size() - 1; i++)
	{
		std::string line = lines[i];

		// Check for invalid '==' case
		if (line.find("==") != std::string::npos)
			throw Exception_Config("Invalid Location Block format, check '='");

		if (line.find('=') != std::string::npos) 
		{
			// Handle key-value pairs
			size_t pos = line.find('=');
			key = line.substr(0, pos);
			value = line.substr(pos + 1);

			// Trim whitespace from key and value
			key = trim(key);
			value = trim(value);

			// Check if key or value is empty
			if (key.empty() || value.empty())
				throw Exception_Config("Invalid Location Block format (1)");
		}
		else
			throw Exception_Config("Invalid Location Block format (2)");

		// Push on the vector of newLocation1
		keys.push_back(key);
		values.push_back(value);
	}
	return (parameters);
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

std::vector<std::vector<std::string>>	Config::ft_checkLocation(const std::string &newLocation0, const std::string &newLocation1, ServerInfo &server)
{
    std::vector<std::vector<std::string>>	locParams;

	locParams = ft_splitLocationParameters(newLocation0, newLocation1);
	if (locParams[0].size() <= 1)
		throw  Exception_Config("Invalid Location Block foramt (3)");
	return (locParams);
}

void	Config::ft_checkErrorPage(const std::string &key, const std::string &value, ServerInfo &server)
{
	// Extract the last three characters from key
	size_t spacePos = key.find(' ');
	std::string errorKey = key.substr(spacePos + 1).substr(0, 3);

	// Check if the first characters of value are '/config/error_page/'
	std::string prefix = "/config/error_page/";
	if (value.substr(0, prefix.size()) != prefix)
		throw  Exception_Config("Invalid Error_page path");

	// Ensure only 8 characters are left after the prefix
	std::string remaining = value.substr(prefix.size());
	if (remaining.size() != 8)
		throw  Exception_Config("Invalid Error_page (1)");

	// Check if the remaining part is one of the valid error pages
	std::vector<std::string> validErrorPages = {
		"400.html", "403.html", "404.html", "405.html", "406.html", "409.html", "410.html", "500.html"
	};
	if (std::find(validErrorPages.begin(), validErrorPages.end(), remaining) == validErrorPages.end())
		throw  Exception_Config("Invalid Error_page (2)");

	/* Compare the last 3 characters of key with the first 3 characters of the error code in value
	 * To insure the key has the correct value
	 * Input1: error_page 400 	=	/config/error_page/400.html
	 * Input2 error_page 400 	=	/config/error_page/409.html
	 * 
	 * Input1: errorKey(400) == errorValue(400) -> correct
	 * Input2: errorKey(400) == errorValue(409) -> false
	 */
	std::string errorValue = remaining.substr(0, 3);
	if (errorKey != errorValue)
		throw  Exception_Config("Invalid Error_page (3)");
}
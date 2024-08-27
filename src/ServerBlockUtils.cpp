#include "ServerBlock.hpp"
#include "Location.hpp"
#include "Config.hpp"

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Helper functions ---
/* Helper function to check the type of the path.
 * - Is path FILE(1), FOLDER(2) or SOMETHING_ELSE(3)
 */
int ServerBlock::getPathType(std::string const path)
{
	struct stat	buffer;
	int			result;

	result = stat(path.c_str(), &buffer);
	if (result == 0) // stat returns 0 on success and -1 on failure
	{
		if (buffer.st_mode & S_IFREG) // 'S_IFREG' is a macro that checks if the file is a regular file
			return (FILE);
		else if (buffer.st_mode & S_IFDIR) // 'S_IFDIR' is a macro that checks if the file is a directory
			return (FOLDER);
		else
			return (SOMETHING_ELSE);
	}
	else
		return (-1);
}

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Checkers functions ---

std::string ServerBlock::ft_checkLocationPath(const std::string &newPath, Location locBlock)
{
	// Check if variable is already set
	if (!locBlock.getPath().empty())
		throw  Exception_ServerBlock("Location_Block: Path is duplicated");

	// Check if the first char is a '/'
	if (newPath[0] != '/')
		throw Exception_ServerBlock("Invalid Location_Block Path: Wrong syntax");

	return newPath;
}

std::string ServerBlock::ft_checkLocationRoot(const std::string &newRoot, Location locBlock, std::string path)
{
	std::string NewFullRoot;

	// Check if variable is already set
	if (!locBlock.getRoot().empty())
		throw  Exception_ServerBlock("Loction_Block: Root is duplicated");

	// Check if the first char is a '/'
	if (newRoot[0] != '/')
		throw Exception_ServerBlock("Invalid Loction_Block Root: Wrong syntax");

	if (ServerBlock::getPathType(newRoot) == FOLDER)
		return newRoot;
	else
	{
		char dir[1024];
		getcwd(dir, 1024);
		NewFullRoot = dir + newRoot + path;
	}
	return NewFullRoot;
}

std::string ServerBlock::ft_checkLocationReturn(const std::string &newRedir, std::string path)
{
	/* '/cgi-bin' should not be redirected
	 * - Security Risks: The cgi-bin directory typically contains scripts and executables that handle form submissions, process data, 
	 *	 and perform other server-side functions. Redirecting it could inadvertently expose these scripts to unintended locations 
	 *	 or users, potentially leading to unauthorized access or execution of scripts.
	 *	 If a cgi-bin directory is redirected, attackers might exploit the redirection to trick the server into executing harmful scripts or commands. */
	if (path == "/cgi-bin")
		throw Exception_ServerBlock("Invalid Location_Block Return: Parameter return not allow for cgi-bin");
	
	// Skip leading whitespace
	size_t pos = 0;
	while (pos < newRedir.size() && std::isspace(newRedir[pos]))
		++pos;

	// Extract the first three non-whitespace characters
	std::string statusCodeStr;
	for (int i = 0; i < 3 && pos < newRedir.size(); ++i)
		statusCodeStr += newRedir[pos++];

	if (statusCodeStr == "301" || statusCodeStr == "302")
		_codeTest = std::stoi(statusCodeStr);
	else
		throw Exception_ServerBlock("Location_Block: return invalid (1)");

	// Skip any whitespace after the status code
	while (pos < newRedir.size() && std::isspace(newRedir[pos]))
		++pos;

	// Return the rest of the string
	std::string remainingRedir = newRedir.substr(pos);
	if (remainingRedir.empty())
		throw Exception_ServerBlock("Location_Block: return invalid (2)"); 

	return remainingRedir;
}

void ServerBlock::ft_checkLocationIndex(const std::string &newIndex, Location locBlock)
{
	// Check if variable is already set
	if (!locBlock.getIndex().empty())
		throw  Exception_ServerBlock("Location_Block: Index is duplicated");

	// Check if the first char is a '/'
	if (newIndex[0] != '/')
		throw Exception_ServerBlock("Invalid Location_Block Index: Wrong syntax");

	// Check if the last 5 characters are '.html'
	if (newIndex.size() < 5 || newIndex.substr(newIndex.size() - 5) != ".html")
		throw Exception_ServerBlock("Invalid Location_Block Index: check your '.html' file");
}

std::vector<std::string> ServerBlock::ft_checkLocationMethods(const std::string &newMethods)
{
	// Split the input string by whitespace
	std::istringstream	iss(newMethods);
	std::vector<std::string>	methods;
	std::string	word;
	bool		oneDELETE	= false;
	bool		oneGET		= false;
	bool 		onePOST		= false;

	while (iss >> word)
	{
		// Check if the word is one of the allowed methods
		if ((word == "DELETE" ) && (oneDELETE == false))
		{
			oneDELETE = true;
			methods.push_back(word);
		}
		else if ((word == "GET") && (oneGET == false))
		{
			oneGET = true;
			methods.push_back(word);
		}
		else if ((word == "POST") && (onePOST == false))
		{
			onePOST = true;
			methods.push_back(word);
		}
		else
			throw Exception_ServerBlock("Invalid Location_Block Method");
	}
	return methods;
}

bool	ServerBlock::ft_checkLoactionAutoindex(const std::string &newAutoindex, std::string path)
{
	/* '/cgi-bin' should not have autoindex ON
	 * - Security Risks: would expose these scripts to users, potentially revealing sensitive information or 
	 *					 leading to security vulnerabilities. */
	if (path == "/cgi-bin")
		throw Exception_ServerBlock("Invalid Location_Block Autoindex: Parameter autoindex not allow for cgi-bin");
	if (newAutoindex == "on")
		return ON;
	else if (newAutoindex == "off")
		return OFF;
	else
		throw Exception_ServerBlock("Invalid Location_Block Autoindex");
}

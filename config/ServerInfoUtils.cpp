#include "ServerInfo.hpp"
#include "Location.hpp"
#include "Config.hpp"

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Helper functions ---
/* Helper function to check the type of the path.
 * - Is path FILE(1), FOLDER(2) or SOMETHING_ELSE(3)
 */
int ServerInfo::getPathType(std::string const path)
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

void ServerInfo::ft_checkLocationPath(const std::string &newPath, Location locBlock)
{
	// Check if variable is already set
	if (!locBlock.getPath().empty())
		throw  Exception_ServerInfo("Location_Block: Path is duplicated");
	


}

std::string ServerInfo::ft_checkLocationRoot(const std::string &newRoot, Location locBlock)
{
	std::string NewFullRoot;

	// Check if variable is already set
	if (!locBlock.getRoot().empty())
		throw  Exception_ServerInfo("Loction_Block: Root is duplicated");

	// Check if the first char is a '/'
	if (newRoot[0] != '/')
		throw Exception_ServerInfo("Invalid Loction_Block Root: Wrong syntax");

	// If the path type is a 2 directory
	if (ServerInfo::getPathType(newRoot) == FOLDER)
		throw Exception_ServerInfo("Invalid Loction_Block Root");
	return newRoot;
}

void ServerInfo::ft_checkLocationIndex(const std::string &newIndex, Location locBlock)
{
	// Check if variable is already set
	if (!locBlock.getIndex().empty())
		throw  Exception_ServerInfo("Index is duplicated");

	// Check if the first char is a '/'
	if (newIndex[0] != '/')
		throw Exception_ServerInfo("Invalid Index: Wrong syntax");

	// Check if the last 5 characters are '.html'
	if (newIndex.size() < 5 || newIndex.substr(newIndex.size() - 5) != ".html")
		throw Exception_ServerInfo("Invalid Index: check your '.html' file");
}

void ServerInfo::ft_checkLocationMethods(const std::string &newMethods, Location locBlock)
{

}

bool	ServerInfo::ft_checkLoactionAutoindex(const std::string &newAutoindex)
{
	// if (path == "/cgi-bin")
	// 			throw ErrorException("Parametr autoindex not allow for CGI");
	if (newAutoindex == "on")
		return ON;
	else if (newAutoindex == "off")
		return OFF;
	else
		throw Exception_ServerInfo("Invalid Location_Block Autoindex");
}



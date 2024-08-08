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
	std::string NewFullPath;

	// Check if variable is already set
	if (!locBlock.getPath().empty())
		throw  Exception_ServerBlock("Location_Block: Path is duplicated");

	// Check if the first char is a '/'
	if (newPath[0] != '/')
		throw Exception_ServerBlock("Invalid Location_Block Path: Wrong syntax");

	// If the path type is a directory -> path needs to be a valid directory  
	if (ServerBlock::getPathType(newPath) == FOLDER)
		return newPath;
	else
	{
		// Concatenates the current working directory (dir) with the given NewRoot path, resulting in a full path
		char dir[1024];
		getcwd(dir, 1024);
		NewFullPath = dir + newPath;
		if (ServerBlock::getPathType(NewFullPath) != FOLDER) // If the path type is not a directory
			throw Exception_ServerBlock("Invalid Loction_Block Path");
	}
	return NewFullPath;
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

	// If the path type is a directory -> path needs to be a valid directory
	if (ServerBlock::getPathType(newRoot) == FOLDER)
		return newRoot;
	else
	{
		NewFullRoot = path + newRoot;;
		if (ServerBlock::getPathType(NewFullRoot) != FOLDER) // If the path type is not a directory
			throw Exception_ServerBlock("Invalid Loction_Block Root");
	}
	return NewFullRoot;
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

std::vector<std::string> ServerBlock::ft_checkLocationMethods(const std::string &newMethods, Location locBlock)
{

	// NOT WORKING YET !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Split the input string by whitespace
	std::istringstream iss(newMethods);
	std::vector<std::string> methods;
	std::string word;

	while (iss >> word)
	{
		// Check if the word is one of the allowed methods
		if (word == "DELETE" || word == "GET" || word == "POST")
			methods.push_back(word);
		else
			throw Exception_ServerBlock("Invalid Location_Block Method");
	}
	return methods;
}

bool	ServerBlock::ft_checkLoactionAutoindex(const std::string &newAutoindex, std::string path)
{
	if (path == "/cgi-bin")
		throw Exception_ServerBlock("Invalid Location_Block Autoindex: Parametr autoindex not allow for CGI");
	if (newAutoindex == "on")
		return ON;
	else if (newAutoindex == "off")
		return OFF;
	else
		throw Exception_ServerBlock("Invalid Location_Block Autoindex");
}



#include "ServerBlock.hpp"
#include "Location.hpp"
#include "Config.hpp"

// --- Constructor ---
ServerBlock::ServerBlock() : _port(0), _serverFd(-1), _maxClient(0), _host(""), _root(""), _index(""), _serverName("") {}

// --- Setter ---
void	ServerBlock::setHost(const std::string &newHost)
{
	_host = newHost;
}

void	ServerBlock::setPort(int newPort)
{
	_port = newPort;
}

void	ServerBlock::setRoot(const std::string &newRoot)
{
	_root = newRoot;
}

void	ServerBlock::setMaxClient(int newMaxClient)
{
	_maxClient = newMaxClient;
}

void	ServerBlock::setIndex(const std::string &newIndex)
{
	_index = newIndex;
}

void	ServerBlock::setServerName(const std::string &newServerName)
{
	_serverName = newServerName;
}

void	ServerBlock::setServerFd(int fd)
{
    _serverFd = fd;
}

void ServerBlock::setLocations(std::vector<std::vector<std::string>> &newLocation, ServerBlock &server, std::string STRING)
{
	(void)server;
	Location locBlock;
	createLocation(newLocation, locBlock, STRING);
	_locations.push_back(locBlock);
}

void ServerBlock::setErrorPage(const std::string &newErrorPage)
{
	// Check if variable is already set
	if (!hasErrorPage(newErrorPage))
		_errorPage.push_back(newErrorPage);
	else
		throw  Exception_ServerBlock("Error_page is duplicated");
}

void	ServerBlock::setLocationsDefault(const Location &defaultLocation)
{
	// Check if locations are empty before setting default
	if (_locations.empty())
		_locations.push_back(defaultLocation);
}

// --- Getter ---
std::string	ServerBlock::getHost() const
{
	return _host;
}

int ServerBlock::getPort() const
{
	return _port;
}

std::string	ServerBlock::getRoot() const
{
	return _root;
}

int ServerBlock::getServerFd() const
{
	return _serverFd;
}

int	ServerBlock::getMaxClient() const
{
	return _maxClient;
}

std::string	ServerBlock::getIndex() const
{
	return _index;
}

std::string	ServerBlock::getServerName() const
{
	return _serverName;
}

std::vector<Location> ServerBlock::getLocations() const
{
	return _locations;
}

std::vector<std::string> ServerBlock::getErrorPage() const
{
	return _errorPage;
}

// --- Utils ---
// Helper function to check fot doubels in error_page
bool ServerBlock::hasErrorPage(const std::string &errorPage) const
{
    return (std::find(_errorPage.begin(), _errorPage.end(), errorPage) != _errorPage.end());
}

// Helper function that checks for that checks for the correct root
void ServerBlock::checkRootRound2(Location locBlock, std::string path)
{
	if (path == "/")
	{
		std::string NewFullRoot;

		char dir[1024];
		getcwd(dir, 1024);
			NewFullRoot = dir;
		locBlock.setRoot(NewFullRoot);
	}
	else	
		throw Exception_ServerBlock("Location_Block: needs a root");
}

/* ft_checkLocation();
* - Checks that all variables are filled
* - If not filled it will get set to the default value.
*/
void	ServerBlock::ft_checkLocationVariables(Location &locBlock, std::string STRING)
{
	if (locBlock.getRoot().empty())
		checkRootRound2(locBlock, locBlock.getPath());
	if (locBlock.getIndex().empty())
		locBlock.setIndex(STRING);
	if (locBlock.getRedir().empty())
	{
		locBlock.setRedirStatusCode(0);
		locBlock.setRedir("");
	}
	if (locBlock.getAllowedMethods().empty())
	{
		std::vector<std::string> defaultMethods = {"DELETE", "POST", "GET"};
		locBlock.setAllowedMethods(defaultMethods);
	}
}

// Helper function to parse location configuration string into a Location object
void ServerBlock::createLocation(std::vector<std::vector<std::string>> &locParams, Location &locBlock, std::string STRING)
{
	bool boolAutoindex = 0;
	bool boolMethods = 0;
	bool boolReturn = 0;

	// Set variables
	for (size_t i = 0; i < locParams[0].size(); i++)
	{
		if (locParams[0][i] == "location")
		{
			locBlock.setPath(ft_checkLocationPath(locParams[1][i], locBlock));
		}
		else if (locParams[0][i] == "root")
		{
			locBlock.setRoot(ft_checkLocationRoot(locParams[1][i], locBlock, locBlock.getPath()));
		}
		else if (locParams[0][i] == "index")
		{
			ft_checkLocationIndex(locParams[1][i], locBlock);
			locBlock.setIndex(locParams[1][i]);
		}
		else if (locParams[0][i] == "return")
		{
			// Check if variable is already set
			if (boolReturn == 1)
				throw Exception_ServerBlock("Loction_Block: Return is duplicated");
			else
			{
				locBlock.setRedir(ft_checkLocationReturn(locParams[1][i], locBlock.getPath()));
				locBlock.setRedirStatusCode(_codeTest);
				boolReturn = 1;
			}
		}
		else if (locParams[0][i] == "allowed_methods")
		{
			// Check if variable is already set
			if (boolMethods == 1)
				throw Exception_ServerBlock("Location_Block: allowed_method is duplicated");
			locBlock.setAllowedMethods(ft_checkLocationMethods(locParams[1][i]));
			boolMethods = 1;
		}
		else if (locParams[0][i] == "autoindex")
		{
			// Check if variable is already set
			if (boolAutoindex == 1)
				throw Exception_ServerBlock("Location_Block: autoindex is duplicated");
			locBlock.setAutoindex(ft_checkLoactionAutoindex(locParams[1][i], locBlock.getPath()));
			boolAutoindex = 1;
		}
		else
			throw Exception_ServerBlock("Invalid Location_Block parameter");
	}

	// Check that all important varabiles are filled.
	ft_checkLocationVariables(locBlock, STRING); 
}
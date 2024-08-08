#include "ServerInfo.hpp"

// --- Constructor ---
ServerInfo::ServerInfo() : _port(0), _serverFd(-1), _maxClient(0), _host(""), _root(""), _index(""), _serverName("") {}

// --- Setter ---
void	ServerInfo::setHost(const std::string &newHost)
{
	_host = newHost;
}

void	ServerInfo::setPort(int newPort)
{
	_port = newPort;
}

void	ServerInfo::setRoot(const std::string &newRoot)
{
	_root = newRoot;
}

void	ServerInfo::setMaxClient(int newMaxClient)
{
	_maxClient = newMaxClient;
}

void	ServerInfo::setIndex(const std::string &newIndex)
{
	_index = newIndex;
}

void	ServerInfo::setServerName(const std::string &newServerName)
{
	_serverName = newServerName;
}

void ServerInfo::setLocations(std::vector<std::vector<std::string>> &newLocation, ServerInfo &server)
{
	Location locBlock;
	createLocation(newLocation,locBlock, server);
	_locations.push_back(locBlock);
}

void ServerInfo::setErrorPage(const std::string &newErrorPage)
{
	// Check if variable is already set
	if (!hasErrorPage(newErrorPage))
		_errorPage.push_back(newErrorPage);
	else
		throw  Exception_ServerInfo("Error_page is duplicated");
}

// --- Getter ---
std::string	ServerInfo::getHost() const
{
	return _host;
}

int ServerInfo::getPort() const
{
	return _port;
}

std::string	ServerInfo::getRoot() const
{
	return _root;
}

int ServerInfo::getServerFd() const
{
	return _serverFd;
}

int	ServerInfo::getMaxClient() const
{
	return _maxClient;
}

std::string	ServerInfo::getIndex() const
{
	return _index;
}

std::string	ServerInfo::getServerName() const
{
	return _serverName;
}

std::vector<Location> ServerInfo::getLocations() const
{
	return _locations;
}

std::vector<std::string> ServerInfo::getErrorPage() const
{
	return _errorPage;
}

// --- Utils ---
// Helper function to check fot doubels in error_page
bool ServerInfo::hasErrorPage(const std::string &errorPage) const
{
    return (std::find(_errorPage.begin(), _errorPage.end(), errorPage) != _errorPage.end());
}

/* ft_checkLocation();
* - Checks that all variables are filled
* - If not filled it will get set to the default value.
*/
void	ServerInfo::ft_checkLocationVariables(Location &locBlock, ServerInfo &server)
{
	// Check if the Location path is empty, if so, set it to the server root
	if (locBlock.getPath().empty())
		locBlock.setPath(server.getRoot());

	// Check if the Location root is empty, if so, set it to the server root
	if (locBlock.getRoot().empty())
		locBlock.setRoot(server.getRoot());

	// Check if the Location index is empty, if so, set it to the server index
	if (locBlock.getIndex().empty())
		locBlock.setIndex(server.getIndex());

	// Check if the allowed methods are empty, if so, set default methods (DELETE, POST, GET)
	if (locBlock.getAllowedMethods().empty())
	{
		std::vector<std::string> defaultMethods = {"DELETE", "POST", "GET"};
		locBlock.setAllowedMethods(defaultMethods);
	}
}

// Helper function to parse location configuration string into a Location object
void ServerInfo::createLocation(std::vector<std::vector<std::string>> &locParams, Location &locBlock, ServerInfo &server)
{
	bool boolAutoindex = 0;
	// Check if variable is already set
	for (size_t i = 0; i < locParams[0].size(); i++)
	{
		if (locParams[0][i] == "location")
		{
			// ft_checkLocationPath(locParams[1][i], locBlock);
			locBlock.setPath(locParams[1][i]);
		}
		else if (locParams[0][i] == "root")
		{
			locBlock.setRoot(ft_checkLocationRoot(locParams[1][i], locBlock));
		}
		else if (locParams[0][i] == "index")
		{
			ft_checkLocationIndex(locParams[1][i], locBlock);
			locBlock.setIndex(locParams[1][i]);
		}
		else if (locParams[0][i] == "allowed_methods")
		{
			// ft_checkLocationMethods(locParams[1][i], locBlock);
			// locBlock.setAllowedMethods(locParams[1][i], locBlock);
			std::cout << "IN allowed_methods" << std::endl;
		}
		else if (locParams[0][i] == "autoindex")
		{
			// Check if variable is already set
			if (boolAutoindex == 1)
				throw Exception_ServerInfo("Location_Block: autoindex is duplicated");
			locBlock.setAutoindex(ft_checkLoactionAutoindex(locParams[1][i]));
			boolAutoindex = 1;
		}
		else
			throw Exception_ServerInfo("Invalid Location_Block parameter");
	}
	// Check that all important varabiles are filled.
	ft_checkLocationVariables(locBlock, server); 

}
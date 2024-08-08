#include "ServerBlock.hpp"

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

void ServerBlock::setLocations(std::vector<std::vector<std::string>> &newLocation, ServerBlock &server)
{
	Location locBlock;
	createLocation(newLocation,locBlock, server);
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

/* ft_checkLocation();
* - Checks that all variables are filled
* - If not filled it will get set to the default value.
*/
void	ServerBlock::ft_checkLocationVariables(Location &locBlock, ServerBlock &server)
{
	if (locBlock.getPath().empty())
		locBlock.setPath(server.getRoot());
	if (locBlock.getRoot().empty())
		locBlock.setRoot(server.getRoot());
	if (locBlock.getIndex().empty())
		locBlock.setIndex(server.getIndex());

	// THINK THERE IS SOMETHIGN WRONG WITH THIS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (locBlock.getAllowedMethods().empty())
	{
		std::vector<std::string> defaultMethods = {"DELETE", "POST", "GET"};
		locBlock.setAllowedMethods(defaultMethods);
	}
}

// Helper function to parse location configuration string into a Location object
void ServerBlock::createLocation(std::vector<std::vector<std::string>> &locParams, Location &locBlock, ServerBlock &server)
{
	bool boolAutoindex = 0;
	// Check if variable is already set
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
		else if (locParams[0][i] == "allowed_methods")
		{
			// NOT WORKING CORRECTLY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			locBlock.setAllowedMethods(ft_checkLocationMethods(locParams[1][i], locBlock));
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
	ft_checkLocationVariables(locBlock, server); 

}
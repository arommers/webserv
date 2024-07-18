#include "ServerInfo.hpp"

// --- Setter ---
void	ServerInfo::setHost(const std::string &newHost)
{
	// ft_checkHost(newHost);
	_host = newHost;
}

void	ServerInfo::setPort(int newPort)
{
	// ft_checkPort(newPort);
	_port = newPort;
}

void	ServerInfo::setRoot(const std::string &newRoot)
{
	// ft_checkRoot(newRoot);
	_root = newRoot;
}

void	ServerInfo::setServerFd(int fd)
{
	// ft_checkServerFd(newServerFd);
	_serverFd = fd;
}

void	ServerInfo::setMaxClient(int newMaxClient)
{
	// ft_checkMaxClient(newMaxClient);
	_maxClient = newMaxClient;
}

void	ServerInfo::setIndex(const std::string &newIndex)
{
	// ft_checkIndex(newIndex);
	_index = newIndex;
}

void	ServerInfo::setServerName(const std::string &newServerName)
{
	// ft_checkServerName(newServerName);
	_serverName = newServerName;
}

void ServerInfo::setLocations(const std::vector<Location> &newLocation)
{
	// ft_checkLocation(newLocation);
	_locations = newLocation;
}

void ServerInfo::setErrorPage(const std::vector<std::string> &newErrorPage)
{
	// ft_checkErrorPage(newErrorPage);
	_errorPage = newErrorPage;
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


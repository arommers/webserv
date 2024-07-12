#include "ServerInfo.hpp"

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

void	ServerInfo::setServerFd(int fd)
{
	_serverFd = fd;
}

void	ServerInfo::setMaxClient(int newMaxClient)
{
	_maxClient = newMaxClient;
}

void	ServerInfo::setIndex(const std::string &newIndex)
{
	_index = newIndex;
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


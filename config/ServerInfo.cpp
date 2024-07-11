#include "ServerInfo.hpp"

// --- Setter ---
void	ServerInfo::setHost(const std::string &newHost)
{
	_host = newHost;
}

void	ServerInfo::setPort(const std::vector<int> &newPort)
{
	_ports = newPort;
}

void	ServerInfo::setRoot(const std::string &newRoot)
{
	_rootPath = newRoot;
}

// --- Getter ---
std::string	ServerInfo::getHost() const
{
	return _host;
}

std::vector<int> ServerInfo::getPort() const
{
	return _ports;
}

std::string	ServerInfo::getRoot() const
{
	return _rootPath;
}

#include "serverInfo.hpp"

// Constructor
ServerInfo::ServerInfo() {}

// Destructor
ServerInfo::~ServerInfo() {}

// --- Setter ---
void	ServerInfo::setName(const std::string &newName)
{
	_serverName = newName;
}

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
std::string	ServerInfo::getName() const
{
	return _serverName;
}

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

#include "ServerInfo.hpp"

// Constructor
ServerInfo::ServerInfo()
{
	std::cout << "SERVER Constructor called !!!!!!" << std::endl; // mute or rm later
}

// Destructor
ServerInfo::~ServerInfo()
{
	std::cout << "SERVER Destructor called !!!!!!" << std::endl; // mute or rm later
}

// --- Setter ---
void ServerInfo::setName(const std::string &newName)
{
	_serverName = newName;
}

void ServerInfo::setPort(const std::vector<int> &newPort)
{
	_ports = newPort;
}

void ServerInfo::setRoot(const std::string &newRoot)
{
	_rootPath = newRoot;
}

// --- Getter ---
std::string ServerInfo::getName() const
{
	return _serverName;
}

std::vector<int> ServerInfo::getPort() const
{
	return _ports;
}

std::string ServerInfo::getRoot() const
{
	return _rootPath;
}
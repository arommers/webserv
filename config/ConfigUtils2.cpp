#include "ServerInfo.hpp"
#include "Location.hpp"
#include "Config.hpp"

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Checkers functions ---
void	Config::ft_checkHost(const std::string &newHost)
{
	// if (!server.getHost().empty())
	// 	throw  Exception_Config("Host is duplicated");
}

void	Config::ft_checkPort(int newPort)
{
	// if (server.getPort())
	// 	throw  Exception_Config("Port is duplicated");
}

void	Config::ft_checkRoot(const std::string &newRoot)
{
	// if (!server.getRoot().empty())
	// 	throw  Exception_Config("Root is duplicated");
}

void	Config::ft_checkMaxClient(int newMaxClient)
{
	// if (server.getMaxClient())
	// 	throw  Exception_Config("max_client_size is duplicated");
}

void	Config::ft_checkIndex(const std::string &newIndex)
{
	// if (!server.getIndex().empty())
	// 	throw  Exception_Config("Index is duplicated");
}

void	Config::ft_checkServerName(const std::string &newServerName)
{
	// if (!server.getServerName().empty())
	// 	throw  Exception_Config("Server_name is duplicated");
}

void	Config::ft_checkLocation(const std::vector<Location> &newLocation)
{

}

void	Config::ft_checkErrorPage(const std::vector<std::string> &newErrorPage)
{

}
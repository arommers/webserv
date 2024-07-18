#ifndef SERVERINFO_HPP
# define SERVERINFO_HPP

# include <iostream>
# include <string.h>
# include <fstream>
# include <vector>
# include "Location.hpp"

class Config;

class ServerInfo
{
	private:
		int							_port;			// A vector of ports the server listens on.
		int							_serverFd;		// Set sever fd to -1.
		int							_maxClient;		// Limit of max clients.
		std::string					_host;			// The host of the server. 				   
		std::string					_root;			// The root directory path for the server. 
		std::string					_index;			// The start html page that will appeare. Also known as the index.
		std::string					_serverName;	// The server name.
		std::vector<Location> 		_locations;		// Inforamtion of the location blocks
		std::vector<std::string>	_errorPage;		// Error Pages

	public:
		ServerInfo() {}		// Constructor
		~ServerInfo() {}	// Destructor

		// Setter
		void setPort(int newPort);
		void setServerFd(int fd);
		void setMaxClient(int newMaxClient);
		void setHost(const std::string &newHost);
		void setRoot(const std::string &newRoot);
		void setIndex(const std::string &newIndex);
		void setServerName(const std::string &newServerName);
		void setLocations(const std::vector<Location> &newLocation);
		void setErrorPage(const std::vector<std::string> &newErrorPage);

		// Getter
		int getPort() const;
		int getServerFd() const;
		int getMaxClient() const;
		std::string getHost() const;
		std::string getRoot() const;
		std::string getIndex() const;
		std::string getServerName() const;
		std::vector<Location> getLocations() const;
		std::vector<std::string> getErrorPage() const;
};

#endif
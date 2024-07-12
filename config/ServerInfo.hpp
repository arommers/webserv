#ifndef SERVERINFO_HPP
# define SERVERINFO_HPP

# include <iostream>
# include <string.h>
# include <fstream>
# include <vector>

class Config;

class ServerInfo
{
	private:
		std::string			_host;			// The host of the server. 				   
		int					_port;			// A vector of ports the server listens on.
		std::string			_root;			// The root directory path for the server. 
		int					_serverFd;		// Set sever fd to -1.
		int					_maxClient;		// Limit of max clients.
		std::string			_index;			// The start html page that will appeare. Also known as the index.
		// _port = port;
		// _serverFd = -1;
		// _maxClient = maxClient;
		// _host = host;
		// _root = root;
		// _index = index; 

	public:
		ServerInfo() {}		// Constructor
		~ServerInfo() {}	// Destructor

		// Setter
		void setHost(const std::string &newHost);
		void setPort(const int &newPort);
		void setRoot(const std::string &newRoot);
		void setServerFd();
		void setMaxClient(const int &newMaxClient);
		void setIndex(const std::string &newIndex);

		// Getter
		std::string getHost() const;
		int getPort() const;
		std::string getRoot() const;
		int getServerFd() const;
		int getMaxClient() const;
		std::string getIndex() const;
};

#endif
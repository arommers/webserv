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
		int					_port;			// A vector of ports the server listens on.
		int					_serverFd;		// Set sever fd to -1.
		int					_maxClient;		// Limit of max clients.
		std::string			_host;			// The host of the server. 				   
		std::string			_root;			// The root directory path for the server. 
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
		void setPort(int newPort);
		void setServerFd(int fd);
		void setMaxClient(int newMaxClient);
		void setHost(const std::string &newHost);
		void setRoot(const std::string &newRoot);
		void setIndex(const std::string &newIndex);

		// Getter
		int getPort() const;
		int getServerFd() const;
		int getMaxClient() const;
		std::string getHost() const;
		std::string getRoot() const;
		std::string getIndex() const;
};

#endif
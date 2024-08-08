#ifndef SERVERINFO_HPP
# define SERVERINFO_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <algorithm>
# include "Location.hpp"

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
		ServerInfo();		// Constructor
		~ServerInfo() {}	// Destructor

		// Setter
		void						setPort(int newPort);
		void						setServerFd(int fd);
		void						setMaxClient(int newMaxClient);
		void						setHost(const std::string &newHost);
		void						setRoot(const std::string &newRoot);
		void						setIndex(const std::string &newIndex);
		void						setServerName(const std::string &newServerName);
		void						setLocations(std::vector<std::vector<std::string>> &newLocation, ServerInfo &server);
		void						setErrorPage(const std::string &newErrorPage);

		// Getter
		int							getPort() const;
		int							getServerFd() const;
		int							getMaxClient() const;
		std::string					getHost() const;
		std::string					getRoot() const;
		std::string					getIndex() const;
		std::string					getServerName() const;
		std::vector<Location>		getLocations() const;
		std::vector<std::string>	getErrorPage() const;


		// Checker
		std::string					ft_checkLocationRoot(const std::string &newRoot, Location locBlock);
		void						ft_checkLocationPath(const std::string &newPath, Location locBlock);
		void						ft_checkLocationIndex(const std::string &newIndex, Location locBlock);
		void						ft_checkLocationMethods(const std::string &newMethods, Location locBlock);
		bool						ft_checkLoactionAutoindex(const std::string &newAutoindex);

		// Utils
		bool						hasErrorPage(const std::string &errorPage) const;
		void						createLocation(std::vector<std::vector<std::string>> &locParams, Location &locBlock, ServerInfo &server);
		void						ft_checkLocationVariables(Location &locBlock, ServerInfo &server);
		int							getPathType(std::string const path);

		// Exception class
		class Exception_ServerInfo: public std::exception
		{
			private:
				const char *_error_msg;

			public:
				Exception_ServerInfo(const char *error_msg) : _error_msg(error_msg) {}	// Constructor
				const char *what() const noexcept										// Overriding the what() method
				{
					return (_error_msg);
				}
		};
}; 

#endif
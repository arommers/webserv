#ifndef ServerBlock_HPP
# define ServerBlock_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <limits.h>
# include <algorithm>
# include <limits.h>
# include "Location.hpp"

class ServerBlock
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
		int				_codeTest;		// Is need to check the redirCode
		ServerBlock();					// Constructor
		~ServerBlock() {}				// Destructor

		// Setter
		void						setPort(int newPort);
		void						setServerFd(int fd);
		void						setMaxClient(int newMaxClient);
		void						setHost(const std::string &newHost);
		void						setRoot(const std::string &newRoot);
		void						setIndex(const std::string &newIndex);
		void						setServerName(const std::string &newServerName);
		void						setLocations(std::vector<std::vector<std::string>> &newLocation, ServerBlock &server, std::string STRING);
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
		std::vector<std::string>	ft_checkLocationMethods(const std::string &newMethods, Location locBlock);
		std::string					ft_checkLocationRoot(const std::string &newRoot, Location locBlock, std::string path);
		std::string					ft_checkLocationPath(const std::string &newPath, Location locBlock);
		std::string					ft_checkLocationReturn(const std::string &newRedir, Location locBlock, std::string path);
		void						ft_checkLocationIndex(const std::string &newIndex, Location locBlock);
		bool						ft_checkLoactionAutoindex(const std::string &newAutoindex, std::string path);

		// Utils
		bool						hasErrorPage(const std::string &errorPage) const;
		void						createLocation(std::vector<std::vector<std::string>> &locParams, Location &locBlock, ServerBlock &server, std::string STRING);
		void						ft_checkLocationVariables(Location &locBlock, ServerBlock &server, std::string STRING);
		void						checkRootRound2(Location locBlock, std::string path);
		int							getPathType(std::string const path);

		// Exception class
		class Exception_ServerBlock: public std::exception
		{
			private:
				const char *_error_msg;

			public:
				Exception_ServerBlock(const char *error_msg) : _error_msg(error_msg) {}	// Constructor
				const char *what() const noexcept										// Overriding the what() method
				{
					return (_error_msg);
				}
		};
}; 

#endif
#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string.h>
# include <fstream>
# include <vector>

// COLORS
# define BLUE "\e[1;34m"
# define RED "\e[1;91m"
# define BOLD "\033[1m"
# define RESET "\033[0m"


# include "ServerInfo.hpp"  // this or Forward declaration?
// class	ServerInfo; // Forward declaration

// configuration file class
class	Config
{
	private:
		std::vector<ServerInfo>		_serverBlocks; 	// Info of each server block 
		int							_server_i;		// How many servers we have
	public:
		Config(std::string file_name);			// Constructor
		~Config();								// Destructor

		// Member functions
		std::string					readConfigFile(std::string name);
		std::string					checkConfigInput(std::string file_content);
		std::vector<ServerInfo>		getServerBlocks();

		// Utils
		void						printConfigFile(); 	// for testing
		void						removeComments(std::string &file_content);
		void						removeWhitespace(std::string &file_content);
		void						splitServers(std::string &file_content);

		// Exception class
		class Exception_Config: public std::exception
		{
			private:
				const char *_error_msg;

			public:
				Exception_Config() : _error_msg("Fatal error") {}					// Default Constructor
				Exception_Config(const char *error_msg) : _error_msg(error_msg) {}	// Constructor
				const char *what() const noexcept									// Overriding the what() method
				{
					return (_error_msg);
				}
		};

};

/*

I need to give adri: private variables

- The name of the server. 				   (std::string)
- A vector of ports the server listens on. (std::vector<int>)
- The root directory path for the server.  (std::string)
- A vector of SocketListen objects. 		// Handles multiple listening sockets -> For later maybe 


_rootDirSettings - Settings for the root directory.
_optDirSettings - Optional directory settings.

*/

#endif
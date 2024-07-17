#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string.h>
# include <fstream>
# include <vector>
# include "ServerInfo.hpp"

// COLORS
# define BLUE "\e[1;34m"
# define RED "\e[1;91m"
# define BOLD "\033[1m"
# define RESET "\033[0m"

// For directory listing (autoindex)
# define ON "true"
# define OFF "false"

// Configuration file class
class	Config
{
	private:
		std::vector<ServerInfo>		_serverBlocks; 	// Info of each server block
		std::vector<std::string>	_info; 			// Save serverBlock string, before they will be parsed correctly 
		int							_server_i;		// How many servers we have

	public:
		Config(std::string file_name);				// Constructor
		~Config() {}								// Destructor

		// Member functions
		std::string					readConfigFile(std::string name);
		void						splitServers(std::string &file_content);
		void 						createServer(std::string &config_string, ServerInfo &server);

		// Getter
		std::vector<ServerInfo>		getServerBlocks();	// Used for creating the sockets (aka. Server.cpp)

		// Utils
		void						ft_printConfigFile();	// for testing purpose
		void						ft_checkServer();
		void						ft_removeComments(std::string &file_content);
		void						ft_removeWhitespace(std::string &file_content);
		bool						ft_checkBrackets(std::string &str);
		size_t						ft_findServerStart(size_t start, std::string &file_content);
		size_t						ft_findServerEnd(size_t start, std::string &file_content);
		std::vector<std::vector<std::string>>	ft_splitParameters(const std::string &config_string);

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

#endif
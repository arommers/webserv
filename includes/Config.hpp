#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string.h>
# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <algorithm>
# include <sys/stat.h>
# include <unistd.h>
# include "ServerBlock.hpp"
# include "Location.hpp"

// COLORS
# define BLUE	"\e[1;34m"
# define RED	"\033[31m"
# define BOLD	"\033[1m"
# define RESET	"\033[0m"

// PATH TYPE
# define FILE 1
# define FOLDER 2
# define SOMETHING_ELSE 3

// Configuration file class
class	Config
{
	private:
		int							_server_i;		// How many servers we have
		std::vector<std::string>	_info;			// Save serverBlock string, before they will be parsed correctly 
		std::vector<ServerBlock>	_serverBlocks;	// Info of each server block

	public:
		Config(std::string file_name);				// Constructor
		~Config() {}								// Destructor

		// Member functions
		std::string								readConfigFile(std::string name);
		void									splitServers(std::string &file_content);
		void 									createServer(std::string &config_string, ServerBlock &server);

		// Getter
		std::vector<ServerBlock>				getServerBlocks() const;	// Used for creating the sockets (aka. Server.cpp)

		// Checkers
		std::string								ft_checkRoot(const std::string &newRoot, ServerBlock &server);
		void									ft_checkHost(const std::string &newHost, ServerBlock &server);
		void									ft_checkPort(const std::string &newPort, ServerBlock &server);
		void									ft_checkMaxClient(const std::string &newMaxClient, ServerBlock &server);
		void									ft_checkIndex(const std::string &newIndex, ServerBlock &server);
		void									ft_checkServerName(const std::string &newServerName, ServerBlock &server);
		void									ft_checkErrorPage(const std::string &param0, const std::string &param1);
		std::vector<std::vector<std::string>>	ft_checkLocation(const std::string &newLocation0, const std::string &newLocation1);

		// Utils
		void									ft_printConfigFile();	// for testing purpose
		void									ft_checkServerVariables(ServerBlock &server);
		void									ft_removeComments(std::string &file_content);
		void									ft_removeWhitespace(std::string &file_content);
		bool									ft_checkBrackets(std::string &str);
		size_t									ft_findServerStart(size_t start, std::string &file_content);
		size_t									ft_findServerEnd(size_t start, std::string &file_content);
		bool									errorPage(std::string string);
		bool									location(std::string string);
		int										getPathType(std::string const path);
		std::vector<std::string>				ft_splitStringByNewline(const std::string &input);
		std::vector<std::vector<std::string>>	ft_splitParameters(const std::string &config_string);
		std::vector<std::vector<std::string>>	ft_splitLocationParameters(const std::string newLocation0, const std::string &newLocation1);

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
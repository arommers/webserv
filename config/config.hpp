#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <vector>

// COLORS
# define BLUE "\e[1;34m"
# define RED "\e[1;91m"
# define BOLD "\033[1m"
# define RESET "\033[0m"

class	Config
{
	private:
		// std::vector<Server>		_server; // need this, so I have the number of servers 
		std::vector<int>		_server;   // using this temporary. 
	public:
		Config(std::string file_name);			// Constructor
		~Config();								// Destructor

		// Member functions
		std::string		readConfigFile(std::string name);
		void			printConfigFile();
		std::string		checkConfigString(std::string config);

};

#endif
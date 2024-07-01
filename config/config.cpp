#include "config.hpp"

// Constructor
Config::Config(std::string file_name)
{
	std::vector<std::string> server_info;
	std::string file_content;

	file_content = readConfigFile(file_name); // read config file
	// server_info = ;
	/*
		// Access server details through getter methods
        std::string serverName = myServer.getServerName();
        std::vector<int> ports = myServer.getPorts();
        std::string rootPath = myServer.getRootPath();
	*/
	if (server_info.size() == 0)
	{
		throw Exception_Config("Invalid config, no server info");
	}
	for (int i = 0; i < server_info.size(); i++)
	{
		// Server S1(server_info[i]); -> myServer
		// Add the server to the config's server block vector
        // _serverBlock.push_back(myServer);
	}
}

// Destructor
Config::~Config()
{
	std::cout << "Config Destructor called" << std::endl; // mute or rm later
}


/*	readConfigFile(); 
 *	- Opens the file
 *	- Gets each line (getline())
 *	- Checks each line for validation.
 *	- If something goes wrong it will throw an exeption with an error message.
 */
std::string Config::readConfigFile(std::string name)
{
	std::ifstream	file(name);
	std::string		file_content;

	if (!file.is_open())
	{
		throw Exception_Config(strerror(errno));
	}
	getline(file, file_content, '\0');
	// file_content = checkConfigInput(file_content); // NOT WORKING YET
	if (file_content.size() == 0)
	{
		throw Exception_Config("Config format is wrong");
	}
	file.close();
	return file_content;
}

/*	checkConfigInput(); 
 *	- .
 */
std::string	checkConfigInput(std::string config)
{

}


// void	Config::printConfigFile()
// {
// 	std::cout << "------ " << BOLD << "Printing Config file" << RESET << " ------" << std::endl << std::endl;
// 	// _server needs to be the information of the server.
	
// 	// size_t	i = -1;
// 	// while (++i < _server_block.size())
// 	// {
// 	// 	std::cout << RED << i << RESET << " : server name is " << _serverBlock[i].getServerName() << std::endl;
// 	// 	std::cout << "root path is : " << _serverBlock[i].getRoot() << std::endl;
// 	// 	std::cout << "port : " << _serverBlock[i].getPorts() << std::endl;
		
// 	// 	// OTHES STUFF


// 	// 	std::cout << BOLD << "----------------------------------" << RESET << std::endl;
// 	// }
// }


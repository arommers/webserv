#include "config.hpp"

// Constructor
Config::Config(std::string file_name)
{
	// std::vector<std::string> server_info;
	std::string file_content;

	file_content = readConfigFile(file_name); // read config file
}

// Destructor
Config::~Config()
{
	std::cout << "Config Destructor called" << std::endl; // mute or rm later
}

//------------------------------------------------------------------------------
// -- Member function --

std::string Config::readConfigFile(std::string name)
{
	std::ifstream	file(name);
	std::string		file_content;

	if (!file.is_open())
	{
		throw std::runtime_error("Exception_StopServer(strerror(errno)"); // ????
		// ? Need a different exception ->because server needs to be stoped
		// is there already a function to stop the server if error?
	}
	getline(file, file_content, '\0');
	file_content = checkConfigInput();
	if (file_content.size() == 0)
	{
		throw std::runtime_error("Exception_StopServer : (Config format is wrong)"); // ????
		// ? Need a different exception ->because server needs to be stoped
		// is there already a function to stop the server if error?
	}
	file.close();
	// printConfigFile();
	return file_content;
}

std::string	checkConfigString(std::string config)
{
	
}

void	Config::printConfigFile()
{
	std::cout << "------ " << BOLD << "Printing Config file" << RESET << " ------" << std::endl << std::endl;
	// _server needs to be the information of the server.
	
	// size_t	i = -1;
	// while (++i < _server.size())
	// {
	// 	std::cout << RED << i << RESET << " : server name is " << _server[i].getServerName() << std::endl;
	// 	std::cout << "root path is : " << _server[i].getRoot() << std::endl;
	// 	std::cout << "port : " << _servers[i].getPorts() << std::endl;
		
	// 	// OTHES STUFF


	// 	std::cout << BOLD << "----------------------------------" << RESET << std::endl;
	// }
}
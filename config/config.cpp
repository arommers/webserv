#include "config.hpp"

// Constructor
Config::Config(std::string file_name)
{
	std::string file_content;

	// read config file
	file_content = readConfigFile(file_name);

	// Split the Servers
	splitServers(file_content);
	// std::cout << "READ : " << file_content << std::endl << std::endl;
	
	// // Create the servers -> parse the info into each ServerInfo class
	// for (size_t i = 0; i < _nb_server; i++)
	// {
	// 	ServerInfo server;
	// 	createServer(_server_config[i], server);
	// 	_servers.push_back(server);
	// }
	// if (_nb_server > 1)
	// 	checkServers();
	// printConfigFile();

}

// Destructor
Config::~Config()
{

}

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Main functions for Config ---

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

	// Checks if the file exist
	if (!file.is_open())
	{
		throw Exception_Config(strerror(errno));
	}
	getline(file, file_content, '\0');
	file_content = checkConfigInput(file_content);
	if (file_content.size() == 0)
	{
		throw Exception_Config("Config format is empty");
	}
	file.close();
	return file_content;
}

/*	checkConfigInput(); 
 *	- Remove comments
 *	- Remove whithspace
 *	- So that we get a 'clean' string
 */
std::string	Config::checkConfigInput(std::string file_content)
{
	removeComments(file_content);
	removeWhitespace(file_content);
	return file_content;
}

// 
std::vector<ServerInfo> Config::getServerBlocks()
{
	return (_serverBlocks);
}

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- ALL functions needed for splitServers for Config ---
// /*	splitServers();
//  *	Spliting servers on separetly strings in vector 
//  */
// void Config::splitServers(std::string &file_content)
// {

// 	}
// }

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Utils for Config ---

/*	removeComments();
 *	Removes the comments from 'char #' to '\n' 
 */
void Config::removeComments(std::string &file_content)
{
	size_t pos;

	// Find the first occurrence of '#'
	pos = file_content.find('#');
	while (pos != std::string::npos)
	{
		size_t pos_end;
		pos_end = file_content.find('\n', pos);	// Find the end of the line (newline character) after '#'
		file_content.erase(pos, pos_end - pos);	// Remove the comment
		pos = file_content.find('#');
	}
}

/*	removeWhitespace();
 * Removes whitespaces in the start, end and in the content (if more than one)
 */
void Config::removeWhitespace(std::string &file_content)
{
	size_t	i = 0;

	// Step 1: Remove leading whitespace (start of content)
	while (file_content[i] && isspace(file_content[i]))
		i++;
	file_content = file_content.substr(i);

	// Step 2: Remove trailing whitespace (end of content)
	i = file_content.length() - 1;
	while (i > 0 && isspace(file_content[i]))
		i--;
	file_content = file_content.substr(0, i + 1);

	// Step 3: Remove multiple internal spaces (inbetween of content)
	bool in_space = false;
	i = 0;
	while (i < file_content.length())
	{
		if (isspace(file_content[i]))
		{
			if (in_space)
				file_content.erase(i, 1);
			else
			{
				in_space = true;
				i++;
			}
		}
		else
		{
			in_space = false;
			i++;
		}
	}
}

/*	Prints the arguments that need to be passed onto the 'Server'.
 *	Used for testing.
 */
void	Config::printConfigFile()
{
	std::cout << "------ " << BOLD << "Printing Config file" << RESET << " ------" << std::endl << std::endl;
	
	size_t	i = -1;
	// loops through all the servers (for multiple servers)
	while (++i < _serverBlocks.size())
	{
		// prints WHAT server #
		std::cout << RED << i << RESET << " Server" << std::endl;

		// prints server name
		std::cout << "Server_name : " << _serverBlocks[i].getName() << std::endl;

		// prints port
		std::cout << "Port : ";
		std::vector<int> port = _serverBlocks[i].getPort();
		for (size_t i = 0; i < port.size(); ++i) 
		{
			std::cout << port[i] << "  ";
		}
		std::cout << std::endl;

		// prints root
		std::cout << "RootPath : " << _serverBlocks[i].getRoot() << std::endl;

		// --- ADD more ---

		std::cout << BOLD << "----------------------------------" << RESET << std::endl;
	}
}

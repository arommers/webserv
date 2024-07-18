#include "Config.hpp"

// Constructor
Config::Config(std::string file_name)
{
	std::string file_content;
	_server_i = 0;

	// read config file
	file_content = readConfigFile(file_name);
	// std::cout << "READ : " << file_content << std::endl << std::endl; // ---> RM

	// Split the Servers (you have each server block)
	splitServers(file_content);
	// std::cout << "Server_i : " << _server_i << std::endl;	// ---> RM
	// for (size_t i = 0; i < _server_i; ++i)					// ---> RM 
	// {														// ---> RM
	// 	std::cout << "Server_Block [" << i << "]: " << _info[i] << std::endl << std::endl; // ---> RM
	// }														// ---> RM

	// Create the servers -> parse the info into each ServerInfo class
	for (size_t i = 0; i < _server_i; i++)
	{
		ServerInfo server;
		createServer(_info[i], server);		// We create server
		_serverBlocks.push_back(server);	// We put serverInfo into _serverBlocks, so we can excess it later
	}
	// ft_printConfigFile();	// for testing -> do we have everything
}

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Main functions for Config ---

/* readConfigFile(); 
 * - Opens the file
 * - Gets each line (getline())
 * - Remove comments & whithspace, so we get a 'clean' string
 * - If something goes wrong it will throw an exeption with an error message.
*/
std::string	Config::readConfigFile(std::string name)
{
	std::ifstream	file(name);
	std::string		file_content;

	// Checks if the file exist
	if (!file.is_open())
	{
		throw Exception_Config(strerror(errno));
	}
	getline(file, file_content, '\0');
	ft_removeComments(file_content);
	ft_removeWhitespace(file_content);
	if (file_content.size() == 0)
	{
		throw Exception_Config("Config format is empty");
	}
	file.close();
	return file_content;
}

/* splitServers();
 * - Checks that all breckets are closed
 * - Finds start and end of server '{}'
 * - Pushes the found server to _serverBlock
 */
void	Config::splitServers(std::string &file_content)
{
	size_t start = 0;
	size_t end = 1;

	if (ft_checkBrackets(file_content) == 0)
		throw Exception_Config("There is problem with the scope, please check '{}'");

	// Loop to find and split each server configuration
	while (start != end && start < file_content.length())
	{
		// Find the start and end
		start = ft_findServerStart(start, file_content);
		end = ft_findServerEnd(start, file_content);

		// Extract the server configuration substring and store it
		_info.push_back(file_content.substr(start, end - start + 1));

		_server_i++;		 // Increase the server count
		start = end + 1;	 // Move start index past the end of the current server configuration
	}
}

/* createServer();
 * - ...
 */
void	Config::createServer(std::string &config_string, ServerInfo &server)
{
	//split in key and value (key = value);
	//loop through array find for 'keyword' matching variable.
	// - fill key value into that variable (setter)
	// - before that check if everything is correct (port has only numbers)
	//check if the important stuff have a value -> ft_checkServer()
	// -----------------------------------------------------------------
	// -----------------------------------------------------------------
	// 		// Must be in the file (Importan once)
	// 		_port;
	// 		_host;		   
	// 		_root; 
	// 		_index
	// -----------------------------------------------------------------
	// -----------------------------------------------------------------

	// std::vector<std::string>	parameters;	
	// std::vector<std::string>	key;
	// std::vector<std::string>	value;
	std::vector<std::vector<std::string>>	parameters;
	// bool _maxClientSize = false;
	// bool _autoindex = OFF; 		// Turn on or off directory listing.

	parameters = ft_splitParameters(config_string); // needs improvment !!!!!!
	// for (size_t i = 0; i < parameters[0].size(); ++i)	// ----> RM
	// {				// ----> RM
    //     std::cout << "Key: " << parameters[0][i] << " -> Value: " << parameters[1][i] << std::endl;
    // }				// ----> RM
	if (parameters[0].size() == 0)
		throw  Exception_Config("Invalid configuration foramt (1)");
	for (size_t i = 0; i < parameters.size(); i++)
	{
		if (parameters[0][i] == "port")
		{
			if (server.getPort())
				throw  Exception_Config("Port is duplicated");
			server.setPort(parameters[1][++i]);
		}
		else if (parameters[0][i] == "host")
		{
			if (!server.getHost().empty())
				throw  Exception_Config("Host is duplicated");
			server.setHost(parameters[1][++i]);
		}
		else if (parameters[0][i] == "root")
		{
			if (!server.getRoot().empty())
				throw  Exception_Config("Root is duplicated");
			server.setRoot(parameters[1][++i]);
		}
		else if (parameters[0][i] == "index" && (i + 1) < parameters.size())
		{
			if (!server.getIndex().empty())
				throw  Exception_Config("Index is duplicated");
			server.setIndex(parameters[0][++i]);
		}
		else if (parameters[0][i] == "error_page")
		{
			
		}
		else if (parameters[0][i] == "location" && (i + 1) < parameters.size())
		{
			// std::string	path;
			// i++;
			// if (parameters[0][i] == "{" || parameters[0][i] == "}")
			// 	throw  Exception_Config("Wrong character in server scope{}");
			// path = parameters[0][i];
			// std::vector<std::string> codes;
			// if (parameters[0][++i] != "{")
			// 	throw  Exception_Config("Wrong character in server scope{}");
			// i++;
			// while (i < parameters.size() && parameters[0][i] != "}")
			// 	codes.push_back(parameters[i++]);
			// server.setLocation(path, codes);
			// if (i < parameters.size() && parameters[0][i] != "}")
			// 	throw  Exception_Config("Wrong character in server scope{}");
		}
		else if (parameters[0][i] == "max_client_size")
		{
			if (server.getMaxClient())
				throw  Exception_Config("max_client_size is duplicated");
			server.setMaxClient(parameters[1][++i]);
		}
		else if (parameters[0][i] == "server_name" && (i + 1) < parameters.size())
		{
			if (!server.getServerName().empty())
				throw  Exception_Config("Server_name is duplicated");
			server.setServerName(parameters[0][++i]);
		}
	}

	// Check that all important varabiles are filled.
	// ft_checkServer();
	server.setServerFd(-1);
	if (!server.getPort())
		throw Exception_Config("Port not found");
	if (server.getHost().empty())
		throw Exception_Config("Host not found");
	if (server.getRoot().empty())
		server.setRoot("./html");
	if (server.getIndex().empty())
		server.setIndex("index.html");
	if (!server.getMaxClient())
		server.setMaxClient(10);

	// if (!server.getErrorPage().empty())

		
	


		// Question to Adri:
		/*
		what if there is no location block?
		*/











	// -----------------------------------------------------------------
	// -----------------------------------------------------------------
	std::cout << "HERE" << std::endl << std::endl; // ---> RM
}

/* getServerBlocks();
 * - A getter for the server blocks (used in the sockets aka. Server.cpp)
 */
std::vector<ServerInfo> Config::getServerBlocks()
{
	return (_serverBlocks);
}

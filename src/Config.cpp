#include "Config.hpp"

// --- Constructor ---
Config::Config(std::string file_name) : _server_i(0), _info(0), _serverBlocks(0)
{
	std::string file_content;

	file_content = readConfigFile(file_name);	// read config file
	splitServers(file_content);					// Split the Servers (to have each server block)

	// Create the servers -> parse the info into each ServerBlock class
	for (size_t i = 0; i < _server_i; i++)
	{
		ServerBlock server;
		createServer(_info[i], server);		// We create server
		_serverBlocks.push_back(server);	// We push ServerBlock into _serverBlocks, so we can excess it later
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
	// if (matchingLocations.size() == 0)
	// {
	// 	matchingLocations.push_back(serverBlock.getLocations()[0]); // ----------------------------------------------------
	// }
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
 * - Substract 'key' and 'value' from the file (ft_splitParameters())
 * - Looks for matching keyword
 * - Does checks on the 'value' of the 'key'
 * - Pushes the 'value' onto the matching variable on ServerBlock class
 */
void	Config::createServer(std::string &config_string, ServerBlock &server)
{
	std::vector<std::vector<std::string>>	parameters;
	bool	locationEncountered = false;

	parameters = ft_splitParameters(config_string);
	if (parameters[0].size() == 0)
		throw  Exception_Config("Invalid configuration foramt (3)");

	for (size_t i = 0; i < parameters[0].size(); i++)
	{
		if (location(parameters[0][i]))
		{
			/* Needs to be at the top, so once a location block is encountered.
			 * But there are more variables after the location block, it is invalid. */

			std::string STRING;
			if (!server.getIndex().empty())
				STRING = server.getIndex();
			else
				STRING = "/index.html";
			locationEncountered = true;
			std::vector<std::vector<std::string>> locParams = ft_checkLocation(parameters[0][i], parameters[1][i], server);
			server.setLocations(locParams, server, STRING);
		}
		else if (locationEncountered)
		{
			// Throw an exception if any non-location parameters are encountered after a location block.
			throw Exception_Config("Invalid configuration format: non-location parameter after location block");
		}
		else if (parameters[0][i] == "port")
		{
			ft_checkPort(parameters[1][i], server);
			server.setPort(std::stoi(parameters[1][i]));
		}
		else if (parameters[0][i] == "host")
		{
			server.setHost(ft_checkHost(parameters[1][i], server));
		}
		else if (parameters[0][i] == "root")
		{
			server.setRoot(ft_checkRoot(parameters[1][i], server));
		}
		else if (parameters[0][i] == "index")
		{
			ft_checkIndex(parameters[1][i], server);
			server.setIndex(parameters[1][i]);
		}
		else if (parameters[0][i] == "max_client_size")
		{
			ft_checkMaxClient((parameters[1][i]), server);
			server.setMaxClient(std::stoi(parameters[1][i]));
		}
		else if (parameters[0][i] == "server_name")
		{
			ft_checkServerName(parameters[1][i], server);
			server.setServerName(parameters[1][i]);
		}
		else if (errorPage(parameters[0][i]))
		{
			ft_checkErrorPage(parameters[0][i], parameters[1][i], server);
			server.setErrorPage(parameters[1][i]);
		}
		else
			throw Exception_Config("Invalid configuration format, invalid keyword");
	}

	// Check that all important varabiles are filled.
	ft_checkServerVariables(server);
}

/* getServerBlocks();
 * - A getter for the server blocks (used in the sockets aka. Server.cpp)
 */
std::vector<ServerBlock> Config::getServerBlocks() const
{
	return (_serverBlocks);
}

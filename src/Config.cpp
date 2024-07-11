#include "../includes/Config.hpp"

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
	// for (size_t i = 0; i < _server_i; ++i)		// ---> RM 
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
	// if (_nb_server > 1)
	// 	checkServers();
	// printConfigFile();	// for testing -> do we have everything
}

// Destructor
Config::~Config() {}

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
	removeComments(file_content);
	removeWhitespace(file_content);
	if (file_content.size() == 0)
	{
		throw Exception_Config("Config format is empty");
	}
	file.close();
	return file_content;
}

// // 
// std::vector<ServerInfo> Config::getServerBlocks()
// {
// 	return (_serverBlocks);
// }

void	Config::createServer(std::string &config, ServerInfo &server)
{
	std::cout << "HERE" << std::endl << std::endl; // ---> RM
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

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Utils for Config ---

/* ft_findServerStart();
 * - Checks for keyword 'server'
 * - Puts start position at '{'
 */
size_t	Config::ft_findServerStart(size_t start, std::string &file_content)
{
	size_t i = start;

	// Check if "server" keyword exists in the content
	while (file_content[i])
	{
		if (file_content[i] == 's')
			break ;
		i++;
	}
	if (file_content.compare(i, 6, "server") != 0)
		throw Exception_Config("No 'server' found in the configuration file");

	// Move past "server" keyword and skip whitespace
	i += 6;
	while (file_content[i] && isspace(file_content[i]))
		i++;

	// Check if the server configuration starts with '{'
	if (file_content[i] == '{')
		return i;
	else
		throw  Exception_Config("Invalid server scope{}");
}

/* ft_findServerEnd();
 * - If an opening brace '{' is found, increment the scope.
 * - If a closing brace '}' is found:
 *    - If scope is 0 we have found the matching closing brace.
 *    - If scope is not 0, decrement scope to indicate leaving a nested block.
 */
size_t	Config::ft_findServerEnd(size_t start, std::string &file_content)
{
	size_t	i = start + 1;
	size_t	scope = 0;

	// Iterate from start index to find the end of a server configuration
	while (file_content[i])
	{
		if (file_content[i] == '{')
			scope++;
		if (file_content[i] == '}')
		{
			if (!scope)
				return i;
			scope--;
		}
		i++;
	}
	return start;
}

/* ft_checkBrackets();
 * - Checks if every '{' is closed 
 */
bool	Config::ft_checkBrackets(std::string &str) 
{
	int stack[100];		// Stack to store open brackets
	int top = -1;		// Initialize stack top to -1
	int openBracket;	// Variable to store the type of open bracket
	int i = 0;			// Loop counter to traverse the input string

	while (str[i] != '\0') 
	{
		if (str[i] == '{') 
		{
			top++;
			stack[top] = str[i];
		} 
		else if (str[i] == '}') 
		{
			if (top == -1) 
				return false;    
			openBracket = stack[top];
			top--;
		}
		if (str[i] == '}' && openBracket != '{') 
			return false; // Return false if brackets don't match
		i++;
	}
	if (top != -1)
		return false;	// If there are unmatched open brackets left on the stack, return false
	return true;
}

/* removeComments();
 * - Removes the comments from 'char #' to '\n' 
 */
void	Config::removeComments(std::string &file_content)
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

/* removeWhitespace();
 * - Removes whitespaces in the start, end and in the content (if more than one)
 */
void	Config::removeWhitespace(std::string &file_content)
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

// Prints the arguments that need to be passed onto the 'Server' -> Used for testing purpose
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

		// prints server host
		std::cout << "host : " << _serverBlocks[i].getHost() << std::endl;

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

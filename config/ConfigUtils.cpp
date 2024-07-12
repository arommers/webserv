#include "Config.hpp"
#include "ServerInfo.hpp"


// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Utils for readConfigFile(); ---

/* ft_removeComments();
 * - Removes the comments from 'char #' to '\n' 
 */
void	Config::ft_removeComments(std::string &file_content)
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

/* ft_removeWhitespace();
 * - Removes whitespaces in the start, end and in the content (if more than one)
 */
void	Config::ft_removeWhitespace(std::string &file_content)
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

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Utils for splitServers() ---

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

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Utils for createServer(); ---

// // ---- !!!!!!!!!!!!!!!! NOT DONE !!!!!!!!!!!!!!!! ----
// /* ft_checkServer();
//  * - ...
//  */
// void	Config::ft_checkServer()
// {
// 		if (!(_Bhost && _Bports && _BrootPath))
// 			throw Exception_Config("Failed server validation, you are missing some configuration file information");
// }






// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Utils for gerneral stuff ---

// Prints the arguments that need to be passed onto the 'Server' -> Used for testing purpose
void	Config::ft_printConfigFile()
{
	std::cout << "------ " << BOLD << "Printing Config file" << RESET << " ------" << std::endl << std::endl;
	
	size_t	i = -1;
	// loops through all the servers (for multiple servers)
	while (++i < _serverBlocks.size())
	{
		// prints WHAT server #
		std::cout << RED << i << RESET << " Server" << std::endl;

		// prints server host
		std::cout << "Host : " << _serverBlocks[i].getHost() << std::endl;

		// prints port
		std::cout << "Port : " << _serverBlocks[i].getPort() << std::endl;

		// prints root
		std::cout << "Root : " << _serverBlocks[i].getRoot() << std::endl;

		// prints root
		std::cout << "ServerFd : " << _serverBlocks[i].getServerFd() << std::endl;

		// prints root
		std::cout << "Max Client : " << _serverBlocks[i].getMaxClient() << std::endl;

		// prints root
		std::cout << "Index : " << _serverBlocks[i].getIndex() << std::endl;

		// --- ADD more ---

		std::cout << BOLD << "----------------------------------" << RESET << std::endl;
	}
}

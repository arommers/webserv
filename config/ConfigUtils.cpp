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
		if (file_content[i] == ' ' || file_content[i] == '\t')
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

/* ft_trim();
 * - Helper function to trim whitespace from both ends of a string
 */
static std::string	ft_trim(const std::string& str)
{
	size_t first;
	size_t	last;

	first = str.find_first_not_of(" \t\n");
	if (first == std::string::npos)
		return ("");
	last = str.find_last_not_of(" \t\n");
	return (str.substr(first, last - first + 1));
}

/* ft_isChar();
 * - Helper function to look for characters between 33 and 126 in ascii
 */
static bool ft_isChar(char c) 
{
	int	ascii_value;

	ascii_value = static_cast<int>(c);
	return (ascii_value >= 33 && ascii_value <= 126);
}

/* ft_splitStringByNewline();
 * - Helper function to look for characters between 33 and 126 in ascii
 */
static	std::vector<std::string> ft_splitStringByNewline(const std::string &input)
{
	std::vector<std::string> result;
	std::istringstream stream(input);
	std::string line;
	
	while (std::getline(stream, line))
		result.push_back(line);
	return result;
}

/* ft_checkEqualSign();
 * - Check if all 'key = value' have a '=' sign
 */
void	Config::ft_checkEqualSign(const std::string& config_string)
{
	size_t		start = 0;
	std::string	current_word;

	while (start < config_string.size())
	{
		size_t		equal_pos = config_string.find('=', start);;
		size_t		word_count = 0;
		bool		isWord = false;

		// Loop through the characters from start to equal_pos (or end of string if '=' not found)
		for (size_t i = start; i < (equal_pos == std::string::npos ? config_string.size() : equal_pos); ++i)
		{
			// Character is found -> update word_count and what the current_word is.
			if (ft_isChar(config_string[i]))
			{
				if (!isWord)
				{
					isWord = true;
					word_count++;
				}
				current_word += config_string[i];
			}
			else if (isspace(config_string[i])) // No character is found
			{
				if (isWord)
				{
					if (current_word == "location") // If the current_word is 'location' stop/return
						return ;
					current_word.clear();
				}
				isWord = false;
			}
		}

		// Check if word_count is 2
		if (word_count != 2)
			throw Exception_Config("Invalid configuration foramt (2)");

		// If no '=' was found, break the loop
		if (equal_pos == std::string::npos)
			break ;
			
		// Move past '='
		start = equal_pos + 1;
	}
}

/* ft_splitLocationBlocks();
 * - Split the location Blocks
 * - key: location ./
 * - value : {root = /html/}
 */
void Config::ft_splitLocationBlocks(std::vector<std::string> &keys, std::vector<std::string> &values, size_t start, const std::string &config_string)
{
	std::string 	key;
	std::string 	value;
	size_t 			brace_start;
	size_t 			brace_end;

	while (start < config_string.size())
	{
		// Find the "location"
		size_t location_pos = config_string.find("location", start);
		if (location_pos == std::string::npos)
			break; 
		start = location_pos + 8; // move past "location" (8)

		// Find the position of '{'
		brace_start = config_string.find('{', start);
		if (brace_start == std::string::npos)
			break;

		// Find the position of '}'
		brace_end = config_string.find('}', brace_start);
		if (brace_end == std::string::npos)
			break;

		// Extract key and value
		key = ft_trim(config_string.substr(start, brace_start - start));
		value = ft_trim(config_string.substr(brace_start + 1, brace_end - brace_start - 1)); // Exclude '{' and '}'

		// Add key and value to their vectors
		keys.push_back(key);
		values.push_back(value);

		// Move start past '}'
		start = brace_end + 1; 
	}
}

/* ft_splitParameters();
 * - Split the configuration string into keys and values
 */
std::vector<std::vector<std::string>>	Config::ft_splitParameters(const std::string &config_string)
{
	std::vector<std::vector<std::string>>	parameters(2);
	std::vector<std::string>		&keys = parameters[0];
	std::vector<std::string>		&values = parameters[1];

	std::string		key;
	std::string		value;


	std::vector<std::string> line;
	line = ft_splitStringByNewline(config_string);
	for (size_t i = 0; i < line.size(); i++)
	{
		std::cout << "line " << line[i] << std::endl;
		// Check equal signs '='
		ft_checkEqualSign(line[i]); // rewrite

	}


	size_t		start = 1;
	while (start < config_string.size())
	{
		// Skip leading whitespace
		while (start < config_string.size() && isspace(config_string[start]))
			start++;

		// Check if 'location' is found
		if ((config_string[start] == 'l') && (config_string[start + 1] == 'o') && (config_string[start + 2] == 'c') && (config_string[start + 3] == 'a') && (config_string[start + 4] == 't') && (config_string[start + 5] == 'i') && (config_string[start + 6] == 'o') && (config_string[start + 7] == 'n') && (config_string[start + 8] == ' '))
		{
			// Handle "location blocks"
			ft_splitLocationBlocks(keys, values, start, config_string);
			break;
		}

		// Find the position of the '=' character
		size_t equal_pos = config_string.find('=', start);
		if (equal_pos == std::string::npos)
			break;

		// Extract the key
		key = ft_trim(config_string.substr(start, equal_pos - start));

		// Find the start/end of the value
		size_t value_start = equal_pos + 1;
		while (value_start < config_string.size() && isspace(config_string[value_start]))
			value_start++;
		size_t value_end = value_start;
		while (value_end < config_string.size() && !isspace(config_string[value_end]))
			value_end++;

		// Extract the value
		value = ft_trim(config_string.substr(value_start, value_end - value_start + 1));

		// Add key and value to their vectors
		keys.push_back(key);
		values.push_back(value);

		// Move start past '='
		start = value_end + 1;
	}
	return (parameters);
}


// // ---- !!!!!!!!!!!!!!!! NOT DONE !!!!!!!!!!!!!!!! ----
// /* ft_checkServer();
//  * - ...
//  */
// void	Config::ft_checkServer(ServerInfo &server)
// {
// 	server.setServerFd(-1);
// 	if (!server.getPort())
// 		throw Exception_Config("Port not found");
// 	if (server.getHost().empty())
// 		throw Exception_Config("Host not found");
// 	if (server.getRoot().empty())
// 		server.setRoot("./html");
// 	if (server.getIndex().empty())
// 		server.setIndex("index.html");
// 	if (!server.getMaxClient())
// 		server.setMaxClient(10);
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
		std::cout << RED << i << RESET << BOLD << " Server " << RESET << std::endl;

		// prints server name
		std::cout << "Server Name : " << _serverBlocks[i].getServerName() << std::endl;

		// prints host
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

		// prints error pages
		std::cout << "Error Page : " << std::endl;
		for (size_t j = 0; j < _serverBlocks[i].getErrorPage().size(); j++)
		{
			std::cout << "	- " << j << " " << _serverBlocks[i].getErrorPage()[j] << std::endl;
		}
		std::cout << std::endl;

		// prints location blocks
		std::cout << "Location Blocks : " << std::endl;
		for (size_t j = 0; j < _serverBlocks[i].getLocations().size(); j++)
		{
			Location loc = _serverBlocks[i].getLocations()[j];
			std::cout << j << " : " << std::endl;
			std::cout << "  Path: " << loc.getPath() << std::endl;
			std::cout << "  Root: " << loc.getRoot() << std::endl;
			std::cout << "  Index: " << loc.getIndex() << std::endl;
			std::cout << "  Allowed Methods: ";
			for (size_t k = 0; k < loc.getAllowedMethods().size(); k++)
			{
				std::cout << "- " << loc.getAllowedMethods()[k] << std::endl;
			}
			std::cout << std::endl;
			std::cout << "  Autoindex: " << (loc.getAutoindex() ? "on" : "off") << std::endl;
		}

		std::cout << BOLD << "----------------------------------" << RESET << std::endl;
	}
}

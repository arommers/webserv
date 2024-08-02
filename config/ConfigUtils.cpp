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

/* ft_splitStringByNewline();
 * - static helper function to split by 'new line' and remove empty lines
 */
static	std::vector<std::string> ft_splitStringByNewline(const std::string &input)
{
	std::vector<std::string> result;
	std::istringstream stream(input);
	std::string line;

	while (std::getline(stream, line))
	{
		// Check if the line is not empty and not just whitespace
		if (!line.empty() && !std::all_of(line.begin(), line.end(), [](char c) { return std::isspace(c); }))
			result.push_back(line);
	}
	return (result);
}

/* ft_splitParameters();
 * - Checks for '='
 * - Seperates line into 'key' and 'value' (key = value) + 'Location Blocks'
 * - Checks that all 'key' parametres and 'value' parametres are not empty
 * - If something is not correct it will theow an exeption
 */
std::vector<std::vector<std::string>>	Config::ft_splitParameters(const std::string &config_string)
{
	std::vector<std::vector<std::string>> parameters(2);
	std::vector<std::string> &keys = parameters[0];
	std::vector<std::string> &values = parameters[1];

	// Split the input string by newline
	std::vector<std::string> lines = ft_splitStringByNewline(config_string);

	for (size_t i = 1; i < lines.size() - 1; i++)
	{
		std::string line = lines[i];
		std::string key;
		std::string value;

		// Trim whitespace from both ends of the line
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);


		// Check for invalid '==' case
		if (line.find("==") != std::string::npos)
			throw Exception_Config("Invalid configuration format, check '='");

		if (line.find("location") == 0 && line.find('{') != std::string::npos)
		{
			// Handle 'location' block
			key = line;
			value.clear();
			while (++i < lines.size() && lines[i].find('}') == std::string::npos)
				value += lines[i] + "\n";
			if (i < lines.size())
				value += lines[i]; // Add the closing '}'
			else
				throw Exception_Config("Invalid configuration format, check 'location block'");
		} 
		else if (line.find('=') != std::string::npos) 
		{
			// Handle regular key-value pairs
			size_t pos = line.find('=');
			key = line.substr(0, pos);
			value = line.substr(pos + 1);

			// Trim whitespace from key and value
			key.erase(0, key.find_first_not_of(" \t"));
			key.erase(key.find_last_not_of(" \t") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);

			// Check if key or value is empty
			if (key.empty() || value.empty())
				throw Exception_Config("Invalid configuration format (1)");
		}
		else
			throw Exception_Config("Invalid configuration format (2)");

		// Push on the vector
		keys.push_back(key);
		values.push_back(value);
	}
	return (parameters);
}

/* ft_checkServer();
 * - Set _serverFd -> needs to be -1
 * - Checks that all necessary variables are filled
 * - _port and _host will error if not filled
 * - The rest will be filled by default
 */
void	Config::ft_checkServerVariables(ServerInfo &server)
{
	if (server.getPort() == 0)
        throw Exception_Config("Port not found");
    if (server.getHost().empty())
        throw Exception_Config("Host not found");
    if (server.getRoot().empty())
        server.setRoot("/www");
    if (server.getIndex().empty())
        server.setIndex("index.html");
    if (server.getServerName().empty())
        server.setServerName("W3bMasters");
    if (server.getMaxClient() == 0)
        server.setMaxClient(5000000);

    // // Check _locations and _errorPage if they must not be empty
    // if (server.getLocations().empty())
    //     throw Exception_Config("Locations not found");
    // if (server.getErrorPage().empty())
    //     throw Exception_Config("Error pages not found");
}

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// --- Utils for gerneral stuff ---

/* Helper function to check the type of the path.
 * - Is path FILE(1), FOLDER(2) or SOMETHING_ELSE(3)
 */
int Config::getPathType(std::string const path)
{
	struct stat	buffer;
	int			result;

	result = stat(path.c_str(), &buffer);
	if (result == 0) // stat returns 0 on success and -1 on failure
	{
		if (buffer.st_mode & S_IFREG) // 'S_IFREG' is a macro that checks if the file is a regular file
			return (FILE);
		else if (buffer.st_mode & S_IFDIR) // 'S_IFDIR' is a macro that checks if the file is a directory
			return (FOLDER);
		else
			return (SOMETHING_ELSE);
	}
	else
		return (-1);
}

// Helper function to check the "error_page" parameter
bool Config::errorPage(std::string string)
{
	std::vector<std::string> validErrorPages = {
		"error_page 400",
		"error_page 403",
		"error_page 404",
		"error_page 405",
		"error_page 406",
		"error_page 409",
		"error_page 410",
		"error_page 500"
	};

	// Check if the input string is in the list of valid error pages
	if (std::find(validErrorPages.begin(), validErrorPages.end(), string) != validErrorPages.end())
		return (true);
	return (false);
}

// Helper function to check the "location" parameter
bool Config::location(std::string string)
{
	std::istringstream iss(string);
	std::vector<std::string> words;
	std::string word;

	while (iss >> word)			// Split the input string into words
		words.push_back(word);
	if (words.size() != 3)		// Check if there are exactly 3 words
		return (false);
	if (words[0] != "location")	// Check if the first word is "location"
		return (false);
	if (words[2] != "{")		// Check if the third word is "{"
		return (false);
	return (true);
}

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

#include "../includes/Client.hpp"


std::string	trimWhiteSpace(std::string& string)
{
	size_t start = string.find_first_not_of(" \n\t\r");
	size_t end = string.find_last_not_of(" \n\t\r");

	return string.substr(start, end - start + 1);
}

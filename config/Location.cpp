#include "Location.hpp"

// --- Constructor ---
Location::Location() : _path(""), _root(""), _index(""), _allowedMethods(0), _autoindex(OFF) {}

// --- Getter ---
std::string Location::getPath() const
{
    return _path;
}

std::vector<std::string> Location::getAllowedMethods() const
{
    return _allowedMethods;
}

std::string Location::getRoot() const
{
    return _root;
}

std::string Location::getIndex() const
{
    return _index;
}

bool Location::getAutoindex() const
{
	return _autoindex;
}

// --- Setter ---
void Location::setPath(const std::string &path)
{
    _path = path;
}

void Location::setAllowedMethods(const std::vector<std::string> &methods)
{
	// NOT WORKINH YET !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	for (int i = 0; i < methods.size(); i++)
		std::cout << "HHHHHHHHHHHHHHHH : " << methods[i] << std::endl;
	// _allowedMethods[0];
	// _allowedMethods[1];
	// _allowedMethods[2];

	// for (size_t i = 0; i < methods.size(); i++)
	// {
	// 	if (methods[i] == "GET")
	// 		_allowedMethods[0] = methods;
	// 	else if (methods[i] == "POST")
	// 		_allowedMethods[1] = methods;
	// 	else if (methods[i] == "DELETE")
	// 		_allowedMethods[2] = methods;
	// }
}

void Location::setRoot(const std::string &root)
{
    _root = root;
}

void Location::setIndex(const std::string &index)
{
    _index = index;
}

void Location::setAutoindex(bool autoindex)
{
	_autoindex = autoindex;
}

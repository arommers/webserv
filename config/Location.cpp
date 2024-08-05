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
    _allowedMethods = methods;
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

#include "Location.hpp"

// Location::Location(std::string& path, std::string& root, std::string& index, std::vector<std::string>& allowedMethods)
//     : _path(path), _allowedMethods(allowedMethods), _root(root), _index(index)
// {
//     _path = path;
//     _root = root;
//     _index = index;
//     _allowedMethods = allowedMethods;
// }

// Location::Location(Location& rhs)
// {
//     _path = rhs._path;
//     _root = rhs._root;
//     _index = rhs._index;
//     _allowedMethods = rhs._allowedMethods;
// }
// Location& Location::operator=(const Location& rhs)
// {
//     if (this != &rhs)
//         _path = rhs._path;
//         _root = rhs._root;
//         _index = rhs._index;
//         _allowedMethods = rhs._allowedMethods;
//     return (*this);
// }

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
void Location::setPath(std::string& path)
{
    _path = path;
}

void Location::setAllowedMethods(std::vector<std::string>& methods)
{
    _allowedMethods = methods;
}

void Location::setRoot(std::string& root)
{
    _root = root;
}

void Location::setIndex(std::string& index)
{
    _index = index;
}

void Location::setAutoindex(bool autoindex)
{
	_autoindex = autoindex;
}
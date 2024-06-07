#include "../includes/Server.hpp"

Server::Server()
{
}

Server::~Server()
{
}

void Server::addClient( int fd )
{
    Client newClient(fd);

    _clients[fd] = newClient;
}

Client& Server::getClient( int fd )
{
    return (_clients[fd]);
}

void Server::removeClient( int fd )
{
    _clients.erase(fd);
}

bool Server::checkCGI( int fd )
{
    if (_clients[fd].getHeaderMap()["Path"].size() >= _locCGI.size())
    {
        if (_clients[fd].getHeaderMap()["Path"].substr(0, _locCGI.size()) == _locCGI)
            return (true);
    }
    return (false);
}
int Server::getMethod( int fd )
{
    if (_clients[fd].getHeaderMap()["Method"] == "POST")
        return (POST);
    if (_clients[fd].getHeaderMap()["Method"] == "GET")
        return (GET);
    else
        return (DELETE);
}
int Server::checkFileFolder( int fd )
{
    if (_clients[fd].getHeaderMap()["Path"].back() == '/')
        return (ISFOLDER);
    else
        return (ISFILE);
}
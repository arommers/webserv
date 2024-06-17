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


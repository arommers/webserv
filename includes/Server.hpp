#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include "../includes/Client.hpp"

class Server
{
private:
    std::unordered_map<int, Client> _clients;
public:
    Server(/* args */);
    ~Server();
    void    addClient( int fd );
    Client&  getClient( int fd );
    void    removeClient( int fd );
};



#endif
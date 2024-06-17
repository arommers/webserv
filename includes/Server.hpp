#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include "../includes/Client.hpp"


enum server
{
    GET = 0,
    POST = 1,
    DELETE = 2,
    ISFILE = 3,
    ISFOLDER = 4,
};

class Server
{
private:
    std::unordered_map<int, Client> _clients;
    std::string _locCGI = "/cgi-bin/";
public:
    Server(/* args */);
    ~Server();
    void    addClient( int fd );
    Client&  getClient( int fd );
    void    removeClient( int fd );

};



#endif
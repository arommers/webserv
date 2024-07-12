#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <sys/wait.h>
#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

class Server;
class Client;

class Cgi
{
private:
    std::vector<pid_t>   _childForks;

public:
    Cgi();
    Cgi(const Cgi& rhs);
    Cgi& operator=(const Cgi& rhs);
    ~Cgi();
    
    char**  createEnv(Server& server, Client& client );
    void    createFork(Server& server, Client& client);
    void    launchScript(Server& server, Client& client);
};
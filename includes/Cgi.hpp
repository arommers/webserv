#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <sys/wait.h>
#include "../includes/Client.hpp"

class Server;
// class Client;

class Cgi
{
private:
    std::vector<pid_t>   _childForks;

public:
    Cgi();
    Cgi(const Cgi& rhs);
    Cgi& operator=(const Cgi& rhs);
    ~Cgi();
    
    bool    checkIfCGI( Client& client );
    void    runCGI( Server& server, Client& client);
    char**  createEnv(Server& server, Client& client );
    void    createPipe(Server& server, Client& client, int* fdPipe);
    void    writeBodyToPipe(Server& server, Client& client);
    void    createFork(Server& server, Client& client);
    void    launchScript(Server& server, Client& client);
    void    readClosePipes(Server& server, Client& client);
    void    redirectToPipes(Server& server, Client& client);
};
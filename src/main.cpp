#include "Client.hpp"
#include "Server.hpp"

int main()
{
    Server server;

    server.createServerSocket();
    std::cout << GREEN << "Webserv started and listening on port: " << PORT << RESET << std::endl;
    server.createPollLoop();
    
    return 0;
}
#include "Client.hpp"
#include "Server.hpp"

int main()
{
    int opt = 1;
    Server server;

    server.createServerInstances();

    server.createServerSockets();
    std::cout << GREEN << "Webserv started and listening on port: " << PORT << RESET << std::endl;
    server.createPollLoop();
    
    return 0;
}
#include "Client.hpp"
#include "Server.hpp"

int main()
{
    int opt = 1;
    Server server;


    server.createServerSocket();
    setsockopt(server.getServerSocket(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    std::cout << GREEN << "Webserv started and listening on port: " << PORT << RESET << std::endl;
    server.createPollLoop();
    
    return 0;
}
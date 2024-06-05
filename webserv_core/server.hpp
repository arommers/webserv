#pragma once

#include <iostream>       // Input and output through streams
#include <cstring>        // Manipulate C-style strings and arrays
#include <arpa/inet.h>    // Provides functions for manipulating IP addresses (htonl, htons)
#include <unistd.h>       // Provides access to the POSIX operating system API (close, read, write)
#include <sys/socket.h>   // Provides declarations for the socket API functions (socket, bind, listen, access)
#include <netinet/in.h>   // Constants and structures needed for internet domain addresses ( sockaddr_in AF_INET)
#include <vector>
#include <unordered_map>

class Server {
    private:

        // Maybe change this to it's own class
        struct Client {
            int         fd;
            std::string writeBuffer;
            size_t      writePos = 0;
        };

        int                             serverSocket;
        int                             addrLen;
        struct sockaddr_in              address;
        std::vector<struct pollfd>      fds;
        std::unordered_map<int, Client> Clients;


    public:
        Server();
        Server(const Server &rhs);
        Server& operator=(const Server& rhs);
        ~Server();

        void    createServerSocket();
        void    createPollLoop();
        void    acceptConnection();
        //      function for closing connections
        void    handleClientData(size_t index);
        void    sendClientData(size_t index);

        //      adding and removing from pollfd array
}
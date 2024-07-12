#pragma once

#include <iostream>       // Input and output through streams
#include <cstring>        // Manipulate C-style strings and arrays
#include <arpa/inet.h>    // Provides functions for manipulating IP addresses (htonl, htons)
#include <unistd.h>       // Provides access to the POSIX operating system API (close, read, write)
#include <sys/socket.h>   // Provides declarations for the socket API functions (socket, bind, listen, access)
#include <netinet/in.h>   // Constants and structures needed for internet domain addresses ( sockaddr_in AF_INET)
#include <poll.h>         // Required for struct pollfd and poll
#include <fcntl.h>
#include <vector>
#include <unordered_map>
#include <csignal>
#include <fstream>        // Temporary for testing  
#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

//pending config parsing these are set

#define PORT 4040
#define MAX_CLIENTS 10
#define TIMEOUT 60
#define BUFFER_SIZE 1024

class Cgi;

class Server
{
    private:
        int                             _serverSocket;
        struct sockaddr_in              _address;
        std::vector<struct pollfd>      _pollFds;
        std::unordered_map<int, Client> _clients;
        Cgi*                             _cgi;

    public:
        Server();
        Server(const Server &rhs);
        Server& operator=(const Server& rhs);
        ~Server();


        void    createServerSocket();
        void    createPollLoop();
        void    acceptConnection();
        void    closeConnection(size_t index);
        void    shutdownServer();
        void    handleClientData(size_t index);
        void    sendClientData(size_t index);
        void    addClient(int fd);
        Client&  getClient(int fd);
        void    removeClient(int fd);
        void    checkTimeout(int time);
        int     getServerSocket();
        bool    isCGI( Client& client );
        void    runCGI( Server& server, Client& client);

        void    handleClientRequest();
        int     checkFile(std::string &file);
        std::string readFile(Client &client);




        void   handleClientRequest(Client &client);


        // temporary chatgpt created test functions
        // std::string parseRequest(const std::string& request);
        // std::string buildResponse(const std::string& content);

        // static void signalHandler(int signal);
};

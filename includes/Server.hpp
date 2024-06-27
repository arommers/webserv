#pragma once

#include "../includes/Client.hpp"

class Server
{
    private:
        int                             _serverSocket;
        struct sockaddr_in              _address;
        std::vector<struct pollfd>      _pollFds;
        std::unordered_map<int, Client> _clients;

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

        int     checkFile(std::string &file);
        void    readFile(Client& client);


        void    handleClientRequest(Client &client);

        void    handleGetRequest(Client &client);
        void    handlePostRequest(Client &client);
        void    handleDeleteRequest(Client &client);



        // temporary chatgpt created test functions
        // std::string parseRequest(const std::string& request);
        // std::string buildResponse(const std::string& content);

        // static void signalHandler(int signal);
};

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
        Client& getClient(int fd);
        void    removeClient(int fd);
        void    checkTimeout(int time);
        int     getServerSocket();

        void    openFile(Client& client);

        void    handleClientRequest(Client &client);

        // void    handleGetRequest(Client &client);
        // void    handlePostRequest(Client &client);
        // void    handleDeleteRequest(Client &client);

        void    handleFileRead(size_t index);
};

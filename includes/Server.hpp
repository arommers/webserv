#pragma once

#include "../includes/Client.hpp"
#include "../includes/ServerBlock.hpp"

class Server
{
    private:
        std::vector <ServerBlock>        _servers;
        std::vector<struct pollfd>      _pollFds;
        std::unordered_map<int, Client> _clients;

    public:
        Server();
        Server(const Server &rhs);
        Server& operator=(const Server& rhs);
        ~Server();

        void    createServerInstances();

        void        addServer(const ServerBlock& ServerBlock);
        void        createServerSockets();
        void        createPollLoop();
        void        acceptConnection(int serverSocket);
        void        handleClientData(size_t index);
        void        openFile(Client& client);
        void        handleFileRead(size_t index);
        void        sendClientData(size_t index);
        void        checkTimeout(int time);
        void        shutdownServer();
        void        closeConnection(size_t index);
        void        removeClient(int fd);
        ServerBlock& getServerBlockByFd(int fd);
        void        addClient(int fd, ServerBlock& ServerBlock);
        Client&     getClient(int fd);

        // currently not implemented
        // void        handleClientRequest(Client &client);
};

#pragma once

#include "../includes/Client.hpp"
#include "../includes/ServerInfo.hpp"

class Server
{
    private:
        std::vector <ServerInfo>        _servers;
        std::vector<struct pollfd>      _pollFds;
        std::unordered_map<int, Client> _clients;

    public:
        Server();
        Server(const Server &rhs);
        Server& operator=(const Server& rhs);
        ~Server();

        void    createServerInstances();

        void        addServer(const ServerInfo& serverInfo);
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
        ServerInfo& getServerInfoByFd(int fd);
        void        addClient(int fd, ServerInfo& serverInfo);
        Client&     getClient(int fd);
        std:: string Server::generateFolderContent(std::string path);

        // currently not implemented
        // void        handleClientRequest(Client &client);
};

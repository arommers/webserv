#pragma once

#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp" 
#include "../includes/ServerBlock.hpp"

class Cgi;

class Server
{
    private:
        std::vector <ServerBlock>        _servers;
        std::vector<struct pollfd>      _pollFds;
        std::unordered_map<int, Client> _clients;
        Cgi                             _cgi;

    public:
        Server();
        Server(const Server &rhs);
        Server& operator=(const Server& rhs);
        ~Server();

        void    createServerInstances();

        void                        addServer(const ServerBlock& ServerBlock);
        void                        createServerSockets();
        void                        createPollLoop();
        void                        acceptConnection(int serverSocket);
        void                        closeConnection(size_t index);
        void                        shutdownServer();
        void                        handleClientData(size_t index);
        void                        sendClientData(size_t index);
        void                        addClient(int fd, ServerBlock& ServerBlock);
        Client&                     getClient(int fd);
        void                        removeClient(int fd);
        void                        checkTimeout(int time);
        int                         getServerSocket();
        std::vector<struct pollfd>  getPollFds();
        void                        removePollFd( int fd );
        void                        handleClientRequest();
        int                         checkFile(std::string &file);
        std::string                 readFile(Client &client);
        void                        openFile(Client& client);
        void                        handleFileRead(size_t index);
        ServerBlock&                 getServerBlockByFd(int fd);

        // currently not implemented
        // void        handleClientRequest(Client &client);
};

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

        void            addServer(const ServerBlock& serverInfo);
        void            createServerSockets();
        void            createPollLoop();
        void            acceptConnection(int serverSocket);
        void            handleClientData(size_t index);
        void            openFile(Client& client);
        void            handleFileRead(size_t index);
        void            sendClientData(size_t index);
        void            checkTimeout(int time);
        void            shutdownServer();
        void            closeConnection(size_t index);
        void            removeClient(int fd);
        ServerBlock&    getServerBlockByFd(int fd);
        void            addClient(int fd, ServerBlock& serverInfo);
        Client&         getClient(int fd);
        std::string     generateFolderContent(std::string path);

        // currently not implemented
        // void        handleClientRequest(Client &client);

        void            removePollFd( int fd );
        std::vector<struct pollfd>      getPollFds();
        void            setServer(std::vector<ServerBlock> serverBlocks);


        void                        handleFdWrite(size_t index);
        int                         getServerSocket();
        void                        addPollFd( int fd, short int events );
        void                        addFileToPoll( Client& client, std::string file );

        // int             checkFile(std::string &file);
        void                        handleClientRequest(Client &client);
        void                        handleDeleteRequest(Client& client);

        void                        removePipe( size_t index );
};

        bool            sortLocations(const Location& a, const Location& b);


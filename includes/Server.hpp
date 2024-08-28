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

        void                        removePollFd( int fd );
        std::vector<struct pollfd>  getPollFds();
        void                        setServer(std::vector<ServerBlock> serverBlocks);
        std::vector<ServerBlock>    getServer() const;



        void                    handleFdWrite(size_t index);
        int                     getServerSocket();
        void                    addPollFd( int fd, short int events );
        void                    addFileToPoll( Client& client, std::string file );
        // void                    handleClientRequest(Client &client);
        void                    handleDeleteRequest(Client& client);

        void                    removePipe( size_t index );

        std::vector<Location>   findMatchingLocations(const std::string& file, ServerBlock& serverBlock);
        std::string             resolveFilePath(const std::string& file, const Location& location, ServerBlock& serverBlock);
        bool                    handleDirectoryRequest(std::string& file, const Location& location, Client& client);
        void                    openRequestedFile(const std::string& file, Client& client);
        bool                    checkAllowedMethod(const Location& location, const std::string& method);
        bool                    checkFileExists(const std::string& path);

        bool                    checkFileAccessRights(const std::string& path);

};

        bool    sortLocations(const Location& a, const Location& b);




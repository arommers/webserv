#pragma once

#include <string>

class ServerInfo
{
    private:
        int         _port;
        int         _serverFd;
        int         _maxClient;
        std::string _host;
        std::string _root;
        std::string _index;

    public:
        ServerInfo();
        ServerInfo(int port, std::string& host, int maxClient, std::string& root, std::string& index);
        ~ServerInfo();

        int getPort() const;
        int getServerFd() const;
        void setServerFd(int fd);
        int getMaxClient() const;
        std::string getHost() const;
        std::string getRoot() const;
        std::string getIndex() const;
};
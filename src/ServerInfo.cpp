        #include "../includes/ServerInfo.hpp"
        
        ServerInfo::ServerInfo() {}
        ServerInfo::ServerInfo(int port, std::string& host, int maxClient, std::string& root, std::string& index)
        {
            _port = port;
            _serverFd = -1;
            _maxClient = maxClient;
            _host = host;
            _root = root;
            _index = index;
        }

        ServerInfo::~ServerInfo(){}

        int ServerInfo::getPort() const
        {
            return _port;
        }

        int ServerInfo::getMaxClient() const
        {
            return _maxClient;
        }

        std::string ServerInfo::getHost() const
        {
            return _host;
        }
        std::string ServerInfo::getRoot() const
        {
            return _root;
        }

        std::string ServerInfo::getIndex() const
        {
            return _index;
        }


        int ServerInfo::getServerFd() const
        {
            return _serverFd;
        }

        void ServerInfo::setServerFd(int fd)
        {
            _serverFd = fd;
        }
        
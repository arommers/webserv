#include "../includes/Server.hpp"
#include "../includes/Config.hpp"

// int _shutdownRequest = false;

Server::Server() {}
Server::~Server() {}
// Server::Server(const Server &rhs) {}
// Server& Server::operator=(const Server& rhs) {}


void    Server::addServer(const ServerBlock& ServerBlock)
{
    _servers.push_back(ServerBlock);
}

void    Server::createServerSockets()
{
    struct sockaddr_in address;
    int addrLen = sizeof(address);
    int opt = 1;

    for (ServerBlock& ServerBlock : _servers)
    {
        int serverSocket;

        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            std::cerr << RED << "Socket creation failed: " << strerror(errno) << RESET << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << GREEN << "Socket created successfully: " << serverSocket << RESET << std::endl;

        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(ServerBlock.getHost().c_str());
        address.sin_port = htons(ServerBlock.getPort());

        if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&address), addrLen) < 0)
        {
            std::cerr << RED << "Bind failed: " << strerror(errno) << RESET << std::endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        std::cout << GREEN <<"Bind successful on port: " << ServerBlock.getPort() << RESET << std::endl;

        if (listen(serverSocket, ServerBlock.getMaxClient()) < 0)
        {
            std::cerr << RED << "Listen failed: " << strerror(errno) << RESET << std::endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        std::cout << GREEN << "Listening on port: " << ServerBlock.getPort() << GREEN << std::endl;

        struct pollfd serverFd;
        serverFd.fd = serverSocket;
        serverFd.events = POLLIN;
        _pollFds.push_back(serverFd);

        ServerBlock.setServerFd(serverSocket);
    }
}

void    Server::createPollLoop()
{
    while (true)
    {
        if (_pollFds.empty())
        {
            std::cerr << RED << "No file descriptors to poll." << RESET << std::endl;
            break;
        }

        int pollSize = poll(_pollFds.data(), _pollFds.size(), -1);
        if (pollSize == -1)
        {
            std::cerr << RED << "Poll failed: " << strerror(errno) << RESET << std::endl;
            shutdownServer();
            exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            if (_pollFds[i].revents & POLLIN)
            {
                if (i < _servers.size())
                    acceptConnection(_pollFds[i].fd);
                else if (_clients.count(_pollFds[i].fd))
                    handleClientData(i);
                else
                    handleFileRead(i);
            }
            else if (_pollFds[i].revents & POLLOUT){
            if (_clients.count(_pollFds[i].fd)){
                    if (getClient(_pollFds[i].fd).getState() == RESPONSE)
                        sendClientData(i);
                    else
                        handleClientData(i);
                }
                else
                    handleFdWrite(i);
            }
            else if (_pollFds[i].revents & POLLHUP){
                removePipe(i);
                break;
            }
        }
    }

}

void    Server::acceptConnection(int serverSocket)
{
    int newSocket;
    struct sockaddr_in clientAddress;
    int addrLen = sizeof(clientAddress);

    if ((newSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), reinterpret_cast<socklen_t*>(&addrLen))) < 0)
        std::cerr << RED << "Accept failed: " << strerror(errno) << RESET << std::endl;
    else
    {    
        std::cout << GREEN << "New connection from: " << inet_ntoa(clientAddress.sin_addr) << ", assigned socket is: " << newSocket << RESET << std::endl;

        struct pollfd clientFd;
        clientFd.fd = newSocket;
        clientFd.events = POLLIN;
        _pollFds.push_back(clientFd);
        
        addClient(newSocket, getServerBlockByFd(serverSocket));
    }
}

void Server::handleFileRead(size_t index)
{
    int fd = _pollFds[index].fd;

    for (std::unordered_map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getReadWriteFd() == fd)
        {
            it->second.readNextChunk();
            if (it->second.getState() == READY || it->second.getState() == ERROR)
            {
                for (auto& value : _pollFds)
                {
                    if (value.fd == it->second.getFd()){
                        value.events = POLLOUT;
                        it->second.setState(RESPONSE);
                    }
                }
                removePollFd(it->second.getReadWriteFd());
                it->second.setReadWriteFd(-1);
            }
        }
    }
}

void    Server::handleFdWrite(size_t index)
{
    int fd = _pollFds[index].fd;

    for (std::unordered_map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getReadWriteFd() == fd)
        {
            it->second.writeNextChunk();
            if (it->second.getState() == READY || it->second.getState() == ERROR)
            {
                for (auto& value : _pollFds)
                {
                    if (value.fd == it->second.getFd()){
                        value.events = POLLOUT;
                    }
                }
                removePollFd(it->second.getReadWriteFd());
                it->second.setReadWriteFd(-1);
            }
        }
    }
}

void    Server::handleClientData(size_t index)
{
    Client &client = getClient(_pollFds[index].fd);

    if (client.getState() == PARSE)
    {
        char    buffer[BUFFER_SIZE];
        int     bytesRead = read(_pollFds[index].fd, buffer, BUFFER_SIZE);
        if (bytesRead < 0)
                std::cerr << RED << "Error reading from client socket: " << strerror(errno) << RESET << std::endl;
        else if(bytesRead == 0)
        {
            std::cout << YELLOW << "Client disconnected, socket fd is: " << RESET << std::endl;
            closeConnection(index);
        }
        else
        {
            client.addToBuffer(std::string(buffer, bytesRead));
            std::time_t now = std::time(nullptr);
            std::tm* local_time = std::localtime(&now);
            if (client.requestComplete())
            {
                client.parseBuffer();
                if (client.getState() != ERROR)
                    client.setState(START);
                _pollFds[index].events = POLLOUT;
            }
        }
    }
    else if (client.getState() == START || client.getState() == READY)
    {
            std::cout << GREEN << "Request Received from socket " << _pollFds[index].fd << ", method: [" << client.getRequestMap()["Method"] << "]" << ", version: [" << client.getRequestMap()["Version"] << "], URI: "<< client.getRequestMap()["Path"] <<  RESET << std::endl;
           
            ServerBlock& serverBlock = client.getServerBlock();
            std::string filePath = client.getRequestMap().at("Path");
            std::vector<Location> matchingLocations = findMatchingLocations(filePath, serverBlock);

            if (!matchingLocations.empty())
            {
                const Location& location = matchingLocations[0];
                std::string method = client.getRequestMap().at("Method");

                if (!checkAllowedMethod(location, method))
                {
                    client.setStatusCode(405);
                    client.setState(ERROR);
                    return;
                }
            }
           
            if (_cgi.checkIfCGI(client) == true)
                _cgi.runCGI(*this, client);
            else if(client.getRequestMap().at("Method") == "GET")
                openFile(client);
            else if(client.getRequestMap().at("Method") == "DELETE")
                handleDeleteRequest(client);
    }
    else if (client.getState() == ERROR)
    {
        addFileToPoll(client, "./config/error_page/" + std::to_string(client.getStatusCode()) + ".html");
        client.setState(READING);
    }
}

void    Server::sendClientData(size_t index)
{
    Client& client = getClient(_pollFds[index].fd);

    client.createResponse();
    std::string writeBuffer = client.getWriteBuffer();

    int bytesSent = send(_pollFds[index].fd, writeBuffer.c_str(), writeBuffer.size(), 0);
    if (bytesSent < 0)
    {
        std::cerr << RED << "Error sending data to client: " << strerror(errno) << RESET << std::endl;
        closeConnection(index);
    }
    else
    {
        client.setWriteBuffer(writeBuffer.substr(bytesSent));
        if (client.getWriteBuffer().empty())
        {
            std::cout << GREEN << "Response sent to client: " << _pollFds[index].fd << RESET << std::endl;
            client.resetClientData(); // Resetting all data of client. Right location?
            closeConnection(index);
        }
    }
}

void    Server::closeConnection(size_t index)
{
    int fd = _pollFds[index].fd;

    close(fd);
    _pollFds.erase(_pollFds.begin() + index);
    removeClient(fd);
}

void    Server::shutdownServer()
{
    while (_pollFds.size() > _servers.size())
        closeConnection(_servers.size());
    for (auto& ServerBlock : _servers)
    {
        if (ServerBlock.getServerFd() != -1)
            close(ServerBlock.getServerFd());
    }
    
    // For now just an insurance. Probably redundant
    _pollFds.clear();
    _clients.clear();
}

Client& Server::getClient(int fd)
{
    return (_clients.at(fd));
}

void    Server::removeClient(int fd)
{
    _clients.erase(fd);
}

std::vector<struct pollfd>  Server::getPollFds()
{
    return (_pollFds);
}

void    Server::addPollFd( int fd, short int events )
{
    struct pollfd pollFdStruct;

    pollFdStruct.fd = fd;
    pollFdStruct.events = events;
    _pollFds.push_back(pollFdStruct);
}

void    Server::removePollFd( int fd )
{
    int i = 0;
    for (auto& value : _pollFds)
    {
        if (value.fd == fd)
        {
            close(fd);
            _pollFds.erase(_pollFds.begin() + i);
            return ;
        }
        i++;
    }
}

void    Server:: removePipe( size_t index )
{
    for (std::unordered_map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getReadWriteFd() == _pollFds[index].fd)
        {
            it->second.setState(RESPONSE);
            removePollFd(it->second.getReadWriteFd());
            it->second.setReadWriteFd(-1);
            return ;
        }
    }
}

ServerBlock&    Server::getServerBlockByFd(int fd)
{
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        if (_servers[i].getServerFd() == fd)
            return _servers[i];
    }
    throw std::runtime_error("ServerBlock with the given fd not found");
}


void    Server::addFileToPoll( Client& client, std::string file )
{
    int fileFd;

    fileFd = open(file.c_str(), O_RDONLY);
    if (fileFd == -1)
    {
        perror("file open");
        exit (1);
    }
    client.setReadWriteFd(fileFd);
    addPollFd(fileFd, POLLIN);
}

void    Server::addClient(int fd, ServerBlock& ServerBlock)
{
    _clients.emplace(fd, Client(fd, ServerBlock));
}

void    Server::handleDeleteRequest(Client& client)
{
    std::string toDelete = client.getRequestMap().at("Path");
    toDelete.erase(0, 1);
    if (remove(toDelete.c_str()) < 0)
    {
        client.setStatusCode(404);
        std::cout << "Error removing file: " << client.getRequestMap().at("Path").c_str() << std::endl;
        return ;
    }
    client.setState(RESPONSE);
}

void    Server::setServer(std::vector<ServerBlock> serverBlocks)
{
    _servers = serverBlocks;
}

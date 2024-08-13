#include "../includes/Server.hpp"

Server::Server() {}
Server::~Server() {}
Server::Server(const Server &rhs)
{
    _servers = rhs._servers;
    _pollFds = rhs._pollFds;
    _clients = rhs._clients;
}

Server& Server::operator=(const Server& rhs)
{
    if (this != &rhs)
        _servers = rhs._servers;
        _pollFds = rhs._pollFds;
        _clients = rhs._clients;
    return (*this);
}

/*********  Hardcoded serverblocks for testing purposes   ************/

void Server::createServerInstances()
{
    ServerInfo server1;
    server1.setPort(4040);
    server1.setIndex("index.html");
    server1.setRoot("./html");
    server1.setHost("127.0.0.1");

    ServerInfo server2;
    server2.setPort(8080);
    server2.setIndex("index.html");
    server2.setRoot("./html1");
    server2.setHost("127.0.0.1");

    _servers.push_back(server1);
    _servers.push_back(server2);
}

/*********************************************************************/

// Adds a new server instance to the server list

void    Server::addServer(const ServerInfo& serverInfo)
{
    _servers.push_back(serverInfo);
}

// Creates server sockets and adds them to the poll loop

void Server::createServerSockets()
{
    struct sockaddr_in address;
    int addrLen = sizeof(address);
    int opt = 1;

    for (ServerInfo& serverInfo : _servers)
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
        address.sin_addr.s_addr = inet_addr(serverInfo.getHost().c_str());
        address.sin_port = htons(serverInfo.getPort());

        if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&address), addrLen) < 0)
        {
            std::cerr << RED << "Bind failed: " << strerror(errno) << RESET << std::endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        std::cout << GREEN <<"Bind successful on port: " << serverInfo.getPort() << RESET << std::endl;

        if (listen(serverSocket, serverInfo.getMaxClient()) < 0)
        {
            std::cerr << RED << "Listen failed: " << strerror(errno) << RESET << std::endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        std::cout << GREEN << "Listening on port: " << serverInfo.getPort() << GREEN << std::endl;

        struct pollfd serverFd;
        serverFd.fd = serverSocket;
        serverFd.events = POLLIN;
        _pollFds.push_back(serverFd);

        serverInfo.setServerFd(serverSocket);
    }
}

// Creates the poll loop to handle events in a non-blocking manner

void Server::createPollLoop()
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
                if (_clients.count(_pollFds[i].fd))
                    sendClientData(i);
                else
                    handleFdWrite(i);
            }
            else if (_pollFds[i].revents & POLLOUT)
                sendClientData(i);
        }
    }

}


void Server::handleFileRead(size_t index)
{
    int fd = _pollFds[index].fd;

    for (std::unordered_map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getFileFd() == fd)
        {
            it->second.readNextChunk();
            if (it->second.getState() == READY)
            {
                for (auto& value : _pollFds)
                {
                    if (value.fd == it->second.getFd())
                        value.events = POLLOUT;
                        it->second.setState(WRITING);
                    }
                }
                removePollFd(it->second.getFileFd());
                it->second.setFileFd(-1);
            }
        }
    }
}

void Server::handleFdWrite(size_t index)
{
    int fd = _pollFds[index].fd;

    for (std::unordered_map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second.getFileFd() == fd)
        {
            it->second.writeNextChunk();
            if (it->second.getState() == READY || it->second.getState() == ERROR)
            {
                for (auto& value : _pollFds)
                {
                    if (value.fd == it->second.getFd()){
                        value.events = POLLIN;
                    }
                }
                removePollFd(it->second.getFileFd());
                it->second.setFileFd(-1);
            }  
        }
    }
}


void    Server::acceptConnection()
{
    int newSocket;
    struct sockaddr_in clientAddress;
    int addrLen = sizeof(clientAddress);

    if ((newSocket = accept(_serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), reinterpret_cast<socklen_t*>(&addrLen))) < 0)
        std::cerr << RED << "Accept failed: " << strerror(errno) << RESET << std::endl;
    else
    {    
        std::cout << GREEN << "New connection from: " << inet_ntoa(_address.sin_addr) << ", assigned socket is: " << newSocket << RESET << std::endl;

        struct pollfd clientFd;
        clientFd.fd = newSocket;
        clientFd.events = POLLIN;
        _pollFds.push_back(clientFd);
        
        addClient(newSocket);
    }
}

void    Server::closeConnection(size_t index)
{
    int fd = _pollFds[index].fd;

    close(fd);
    _pollFds.erase(_pollFds.begin() + index);
    removeClient(fd);
}

// void Server::handleClientData(size_t index)
// {
//     char buffer[BUFFER_SIZE];
//     int bytesRead = read(_pollFds[index].fd, buffer, BUFFER_SIZE);

//     if (bytesRead < 0)
//         std::cerr << RED << "Error reading from client socket: " << strerror(errno) << RESET << std::endl;
//     else if (bytesRead == 0)
//     {
//         std::cout << YELLOW << "Client disconnected, socket fd is: " << _pollFds[index].fd << RESET << std::endl;
//         closeConnection(index);
//     }
//     else
//     {
//         buffer[bytesRead] = '\0';
//         Client &client = getClient(_pollFds[index].fd);
//         client.addToBuffer(buffer);

//         if (client.requestComplete())
//         {
//             client.parseBuffer();
//             if (client.getState() == ERROR){
//                 client.createResponse();
//                 return;
//             }
//             std::cout << GREEN << "Request Received from socket " << _pollFds[index].fd << ", method: [" << client.getRequestMap()["Method"] << "], version: [" << client.getRequestMap()["Version"] << "], URI: " << client.getRequestMap()["Path"] << RESET << std::endl;
//             if (_cgi.checkIfCGI(client) == true){
//                 _cgi.runCGI(*this, client);
//                 _pollFds[index].events = POLLOUT; // CGI finished, so POLLOUT can be set
//             }
//             else{
//                 openFile(client);
//             }
//             if (client.getState() == ERROR)
//             {
//                 client.createResponse();
//                 _pollFds[index].events = POLLOUT;
//                 return ;
//             }
//         }
//     }
// }

void    Server::handleClientData(size_t index)
{
    Client &client = getClient(_pollFds[index].fd);

    if (client.getState() == START)
    {
        char    buffer[BUFFER_SIZE];
        int     bytesRead = read(_pollFds[index].fd, buffer, BUFFER_SIZE - 1);
        if (bytesRead < 0)
                std::cerr << RED << "Error reading from client socket: " << strerror(errno) << RESET << std::endl;
        else if(bytesRead == 0)
        {
            std::cout << YELLOW << "Client disconnected, socket fd is: " << RESET << std::endl;
            closeConnection(index);
        }
        else
        {
            buffer[bytesRead] = '\0';

            client.addToBuffer(buffer);
            std::time_t now = std::time(nullptr);
            std::tm* local_time = std::localtime(&now);
            if (client.requestComplete())
            {

                client.parseBuffer();
                if (client.getState() != ERROR)
                    client.setState(READY);
            }
        }
    }
    if (client.getState() != START && client.getState() != ERROR)
    {
            std::cout << GREEN << "Request Received from socket " << _pollFds[index].fd << ", method: [" << client.getRequestMap()["Method"] << "]" << ", version: [" << client.getRequestMap()["Version"] << "], URI: "<< client.getRequestMap()["Path"] <<  RESET << std::endl;
            if (_cgi.checkIfCGI(client) == true){
                _cgi.runCGI(*this, client);
                _pollFds[index].events = POLLOUT; // CGI finished, so POLLOUT can be set
            }
            else{
                openFile(client);
            }
    }
    if (client.getState() == ERROR)
    {
        _pollFds[index].events = POLLOUT;
        return ;
    }
  
    

}

void Server::sendClientData(size_t index)
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


void Server::openFile(Client &client)
{
    int fileFd;
    std::string file;
    
    file = client.getRequestMap().at("Path");
    if (file == "/")
        file += "index.html";
    file = "./html" + file;
    
    std::cout << "Constructed file path: " << file << std::endl;
    fileFd = open(file.c_str(), O_RDONLY);
    if (fileFd < 0)
    {
        client.setStatusCode(404);
        std::cerr << "Failed to open file: " << file << ": " << strerror(errno) << std::endl;
        return;
    }

    client.setFileFd(fileFd);

    struct pollfd pollFd;
    pollFd.fd = fileFd;
    pollFd.events = POLLIN;
    _pollFds.push_back(pollFd);

    client.setState(READING);
}

void    Server::checkTimeout(int time)
{
    std::time_t currentTime = std::time(nullptr);

    for (size_t i = 1; i < _pollFds.size(); ++i)
    {
        Client& client = getClient(_pollFds[i].fd);

        if (difftime(currentTime, client.getTime()) > time)
        {
            std::cout << YELLOW << "Connection timeout, closing socket fd: " << _pollFds[i].fd << RESET << std::endl;
            closeConnection(i);
            --i;
        }
    }
}

void Server::shutdownServer()
{
    // Close all client connections first
    while (_pollFds.size() > _servers.size())
        closeConnection(_servers.size());
    // Close all server sockets
    for (auto& serverInfo : _servers)
    {
        if (serverInfo.getServerFd() != -1)
            close(serverInfo.getServerFd());
    }
    
    // For now just an insurance to. Probably redundant
    _pollFds.clear();
    _clients.clear();
}

void Server::removeClient(int fd)
{
    _clients.erase(fd);
}

std::vector<struct pollfd>  Server::getPollFds()
{
    return (_pollFds);
}

void Server::removePollFd( int fd )
{
    int i = 0;
    for (auto& value : _pollFds)
    {
        if (value.fd == fd){
            _pollFds.erase(_pollFds.begin() + i);
            return ;
        }
        i++;
    }

}
// Retrieve the ServerInfo object corresponding to a given file descriptor

ServerInfo& Server::getServerInfoByFd(int fd)
{
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        if (_servers[i].getServerFd() == fd)
            return _servers[i];
    }
    throw std::runtime_error("ServerInfo with the given fd not found");
}

void Server::addClient(int fd, ServerInfo& serverInfo)
{
    _clients.emplace(fd, Client(fd, serverInfo));
}

Client& Server::getClient(int fd)
{
    return _clients.at(fd);
}




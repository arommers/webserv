#include "../includes/Server.hpp"

// int _shutdownRequest = false;

Server::Server() {}
Server::~Server() {}
// Server::Server(const Server &rhs) {}
// Server& Server::operator=(const Server& rhs) {}


int     Server::getServerSocket()
{
    return (_serverSocket);
}
void    Server::createServerSocket()
{
    int addrLen = sizeof(_address);
    struct pollfd serverFd;


    // Creating socket file descriptor
    if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << RED << "Socket failed: " << strerror(errno) << RESET << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configuring the address struct of the socket
    _address.sin_family = AF_INET; // address family
    _address.sin_addr.s_addr = INADDR_ANY; // accepts connections from any IP on the host
    _address.sin_port = htons(PORT); // ensures the port number is correctly formatted

    // Binding the socket to the address and the port
    if (bind(_serverSocket, reinterpret_cast<struct sockaddr*>(&_address), addrLen) < 0)
    {
        std::cerr << RED << "Bind failed: " << strerror(errno) << RESET << std::endl;
        close(_serverSocket);
        exit(EXIT_FAILURE);
    }

    // Socket starts listening mode
    if (listen(_serverSocket, MAX_CLIENTS) < 0)
    {
        std::cerr << RED << "Listen failed: " << strerror(errno) << RESET << std::endl;
        close(_serverSocket);
        exit(EXIT_FAILURE);
    }

    // // Set socket options
    // int yes = 1;
    // if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    //     std::cerr << RED << "setsockopt failed: " << strerror(errno) << RESET << std::endl;
    //     close(_serverSocket);
    //     exit(EXIT_FAILURE);
    // }

    // // Set non-blocking mode for _serverSocket
    // int flags = fcntl(_serverSocket, F_GETFL, 0);
    // if (flags == -1)
    // {
    //     std::cerr << RED << "fcntl F_GETFL failed for server socket: " << strerror(errno) << RESET << std::endl;
    //     close(_serverSocket);
    //     exit(EXIT_FAILURE);
    // }
    // if (fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK) == -1)
    // {
    //     std::cerr << RED << "fcntl F_SETFL O_NONBLOCK failed for server socket: " << strerror(errno) << RESET << std::endl;
    //     close(_serverSocket);
    //     exit(EXIT_FAILURE);
    // }

    // Add the server socket to the poll file descriptor set
    serverFd.fd = _serverSocket;
    serverFd.events = POLLIN;
    _pollFds.push_back(serverFd);
}

void    Server::createPollLoop()
{
        // signal(SIGINT, signalHandler);
        
        while (true)
        {
            int pollSize = poll(_pollFds.data(), _pollFds.size(), -1);
            if (pollSize == -1)
            {
                std::cerr << RED << "Poll failed: " << strerror(errno) << RESET << std::endl;
                shutdownServer();
                exit(EXIT_FAILURE);
            }

            checkTimeout(TIMEOUT);

            for (size_t i = 0; i < _pollFds.size(); ++i)
            {
                if (_pollFds[i].revents & POLLIN)
                {
                    if (_pollFds[i].fd == _serverSocket)
                        acceptConnection();
                    else
                        handleClientData(i);
                }
                else if (_pollFds[i].revents & POLLOUT)
                    sendClientData(i);
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
        // // Set non-blocking mode for client sockets
        // int flags = fcntl(newSocket, F_GETFL, 0);
        // if (flags == -1) {
        //     std::cerr << RED << "fcntl F_GETFL failed for client socket: " << strerror(errno) << RESET << std::endl;
        //     close(newSocket);
        //     return;
        // }
        // if (fcntl(newSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
        //     std::cerr << RED << "fcntl F_SETFL O_NONBLOCK failed for client socket: " << strerror(errno) << RESET << std::endl;
        //     close(newSocket);
        //     return;
        // }
        
        std::cout << GREEN << "New connection from: " << inet_ntoa(_address.sin_addr) << ", assigned socket is: " << newSocket << RESET << std::endl;

        struct pollfd clientFd;
        clientFd.fd = newSocket;
        clientFd.events = POLLIN | POLLOUT;
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

void    Server::handleClientData(size_t index)
{
    char    buffer[BUFFER_SIZE];
    int     bytesRead = read(_pollFds[index].fd, buffer, BUFFER_SIZE);

    if (bytesRead <= 0)
    {
        if (bytesRead < 0)
            std::cerr << RED << "Error reading from client socket: " << strerror(errno) << RESET << std::endl;
        else
        {
            std::cout << YELLOW << "Client disconnected, socket fd is: " << RESET << std::endl;
            closeConnection(index);
        }
    }
    else
    {
        buffer[bytesRead] = '\0';
        Client &client = getClient(_pollFds[index].fd);
        client.addToBuffer(buffer);
        
        if (client.requestComplete())
        {
            client.parseBuffer();            
            // std::cout << GREEN << "Request Received from socket " << _pollFds[index].fd << ", method: [" << client.getRequestMap()["Method"] << "]" << RESET << std::endl;
        }
        else
            getClient(_pollFds[index].fd).addToBuffer(buffer);
    }
}

void Server::sendClientData(size_t index)
{
    Client& client = getClient(_pollFds[index].fd);

    if(!client.getResponseStatus())
        handleClientRequest(client);
    else if (client.getResponseStatus())
    {
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
                closeConnection(index);
            }
        }
    }
}

void Server::handleClientRequest(Client &client)
{
    std::map<std::string, std::string> requestMap = client.getRequestMap();
    std::map<std::string, std::string>::iterator it = requestMap.find("Method");

    if (it != requestMap.end()) {
        std::string method = it->second;
        if (method == "GET")
            readFile(client);
        // Handle other methods (POST, DELETE) as needed
    } else {
        std::cerr << RED << "Request missing Method key." << RESET << std::endl;
        client.setStatusCode(400); // Bad Request
        client.createResponse();
    }
}

// void Server::handleClientRequest(Client &client)
// {
//     {
//         std::string method = client.getRequestMap().at("Method");

//         if (method == "GET")
//             readFile(client);
//     }
// }

void    Server::readFile(Client &client)
{
    int         fileFd;      
    std::string file;

    client.setStatusCode(200);
    file =  client.getRequestMap().at("Path");
    if (file == "/")
        file += "index.html";
    file = "./html" + file;
    fileFd = open(file.c_str(), O_RDONLY);
    if (fileFd < 0)
    {
        client.setStatusCode(404);
        std::cerr << "failed to open file: " << file << ": " << strerror(errno) << std::endl;
    }
    client.setFd(fileFd);
    client.readNextChunk();
    // close(fileFd);
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

void    Server::shutdownServer()
{
    while (_pollFds.size() > 1)
        closeConnection(1);
    if (_serverSocket != -1)
    close(_serverSocket);
}

void Server::addClient(int fd)
{
    _clients.emplace(fd, Client(fd));
}

Client& Server::getClient(int fd)
{
    return (_clients.at(fd));
}

void Server::removeClient(int fd)
{
    _clients.erase(fd);
}

void Server::addPollFd(int fd, type type)
{
    pollfd pollFd;
    pollFd.fd = fd;

    if (type == type::CLIENT_SOCKET) {
        pollFd.events = POLLIN | POLLOUT;
    } else {
        pollFd.events = POLLIN;
    }

    pollInfo pollInfo;
    pollInfo.fd = pollFd;
    pollInfo.type = type;
    _pollFds.push_back(pollInfo);
}

void Server::removePollFd(int fd)
{
    for (auto it = _pollFds.begin(); it != _pollFds.end(); ++it)
    {
        if (it->fd.fd == fd)
        {
            _pollFds.erase(it);
            break;
        }
    }
}

// void Server::signalHandler(int signal)
// {
//       std::cout << GREEN << "SIGINT received. Shutting down gracefully..." << RESET << std::endl;
//     _shutdownRequest = true;
// }

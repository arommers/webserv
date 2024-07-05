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

/*  CREATES A SERVERSOCKET, INITIALIZES THE POLLFD ARRAY
    AND STARTS LISTENING FOR CONNECTIONS    */

void    Server::createServerSocket()
{
    int addrLen = sizeof(_address);
    struct pollfd serverFd;
    int opt = 1;

    if ((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << RED << "Socket failed: " << strerror(errno) << RESET << std::endl;
        exit(EXIT_FAILURE);
    }
    setsockopt(getServerSocket(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    _address.sin_family = AF_INET; // address family
    _address.sin_addr.s_addr = INADDR_ANY; // accepts connections from any IP on the host
    _address.sin_port = htons(PORT); // ensures the port number is correctly formatted

    if (bind(_serverSocket, reinterpret_cast<struct sockaddr*>(&_address), addrLen) < 0)
    {
        std::cerr << RED << "Bind failed: " << strerror(errno) << RESET << std::endl;
        close(_serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(_serverSocket, MAX_CLIENTS) < 0)
    {
        std::cerr << RED << "Listen failed: " << strerror(errno) << RESET << std::endl;
        close(_serverSocket);
        exit(EXIT_FAILURE);
    }

    serverFd.fd = _serverSocket;
    serverFd.events = POLLIN;
    _pollFds.push_back(serverFd);
}

/*  START A POLL LOOP AND CHECKS FOR REVENTS THAT TRIGGERED
    POLLIN
    - IF IT'S A SERVER SOCKET A NEW CLIENT SOCKET GETS CREATED AND ADDED TO THE POLLFD ARRAY
    - IF IT'S A CLIENT SOCKET 'x' GET READ FROM THE FD AND STORED IN A STRING UNTIL THE REQUEST IS COMPLETE
    - IF IT'S A FILE FD, READ FROM THE FILE UNTILL WE REACH EOF 
    POLLOUT
    - SEND DATA */
     

void    Server::createPollLoop()
{
    while (true)
    {
        int pollSize = poll(_pollFds.data(), _pollFds.size(), -1);
        if (pollSize == -1)
        {
            std::cerr << RED << "Poll failed: " << strerror(errno) << RESET << std::endl;
            shutdownServer();
            exit(EXIT_FAILURE);
        }

        // checkTimeout(TIMEOUT);

        // for (size_t i = 0; i < _pollFds.size(); ++i)
        // {
        //     if (_pollFds[i].revents & POLLIN)
        //     {
        //         if (_pollFds[i].fd == _serverSocket)
        //             acceptConnection();
        //         else
        //             handleClientData(i);
        //     }
        //     else if (_pollFds[i].revents & POLLOUT)
        //         sendClientData(i);

        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            std::cout << "FD: " << _pollFds[i].fd << " revent: " << _pollFds[i].revents << std::endl;
            if (_pollFds[i].revents & POLLIN)
            {
                if (_pollFds[i].fd == _serverSocket)
                    acceptConnection();
                else if (_clients.count(_pollFds[i].fd))
                    handleClientData(i);
                else
                    handleFileRead(i);
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
            if (it->second.getResponseStatus() == true)
            {
                _pollFds[it->second.getFd()].events = POLLOUT;
                std::cout << "TO SEND: \n" << it->second.getWriteBuffer();
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

void    Server::handleClientData(size_t index)
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
        buffer[bytesRead] = '\0';
        Client &client = getClient(_pollFds[index].fd);
        client.addToBuffer(buffer);
        
        if (client.requestComplete())
        {
            client.parseBuffer();
            // We need a check to assess the method
            std::cout << GREEN << "Request Received from socket " << _pollFds[index].fd << ", method: [" << client.getRequestMap()["Method"] << "]" << ", version: [" << client.getRequestMap()["Version"] << "]" <<  RESET << std::endl;
            openFile(client);
        }
    }
}

void Server::sendClientData(size_t index)
{
    Client& client = getClient(_pollFds[index].fd);

    // if(!client.getResponseStatus())
    //     handleClientRequest(client);
    // else if (client.getResponseStatus())
    // {
    std::string writeBuffer = client.getWriteBuffer();

    std::cout << RED << "TEST TEST TEST" << RESET << std::endl;

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
    // }
}

// void Server::handleClientRequest(Client &client)
// {
//     std::map<std::string, std::string> requestMap = client.getRequestMap();
//     std::map<std::string, std::string>::iterator it = requestMap.find("Method");

//     if (it != requestMap.end()) {
//         std::string method = it->second;
//         if (method == "GET")
//             readFile(client);
//         // Handle other methods (POST, DELETE) as needed
//     } else {
//         std::cerr << RED << "Request missing Method key." << RESET << std::endl;
//         client.setStatusCode(400); // Bad Request
//         client.createResponse();
//     }
// }

void Server::openFile(Client &client)
{
    int fileFd;
    std::string file;
    
    file = client.getRequestMap().at("Path");
    if (file == "/") file += "index.html";
        file = "./html" + file;
    
    fileFd = open(file.c_str(), O_RDONLY);
    if (fileFd < 0)
    {
        client.setStatusCode(404);
        std::cerr << "Failed to open file: " << file << ": " << strerror(errno) << std::endl;
        client.createResponse();
        return;
    }

    client.setFileFd(fileFd);

    struct pollfd pollFd;
    pollFd.fd = fileFd;
    pollFd.events = POLLIN;
    _pollFds.push_back(pollFd);
}

// void    Server::readFile(Client &client)
// {
//     int         fileFd;      
//     std::string file;

//     client.setStatusCode(200);
//     file =  client.getRequestMap().at("Path");
//     if (file == "/")
//         file += "index.html";
//     file = "./html" + file;
//     fileFd = open(file.c_str(), O_RDONLY);
//     if (fileFd < 0)
//     {
//         client.setStatusCode(404);
//         std::cerr << "failed to open file: " << file << ": " << strerror(errno) << std::endl;
//     }
//     client.setFd(fileFd);
//     client.readNextChunk();
//     // close(fileFd);
// }

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



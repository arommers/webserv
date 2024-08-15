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

        // possibly use an alternative way to recognize distinction between the different FDs
        for (size_t i = 0; i < _pollFds.size(); ++i)
        {
            if (_pollFds[i].revents & POLLIN)
            {
                if (i < _servers.size())
                    acceptConnection(_pollFds[i].fd);
                else if (_clients.count(_pollFds[i].fd))
                    handleClientData(i);
                else
                // Do we need a distinction between reading from a pipe vs a file?
                    handleFileRead(i);
            }
            else if (_pollFds[i].revents & POLLOUT)
            // need to have a check if the fd is a pipe
            // if so, write to the pipe
                sendClientData(i);
        }
    }
}

// Accept a new connection from a client and add it to the poll loop

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
        
        addClient(newSocket, getServerInfoByFd(serverSocket));
    }
}

void Server::handleClientData(size_t index)
{
    char buffer[BUFFER_SIZE];
    int bytesRead = read(_pollFds[index].fd, buffer, BUFFER_SIZE);

    if (bytesRead < 0)
        std::cerr << RED << "Error reading from client socket: " << strerror(errno) << RESET << std::endl;
    else if (bytesRead == 0)
    {
        std::cout << YELLOW << "Client disconnected, socket fd is: " << _pollFds[index].fd << RESET << std::endl;
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
            std::cout << GREEN << "Request Received from socket " << _pollFds[index].fd << ", method: [" << client.getRequestMap()["Method"] << "], version: [" << client.getRequestMap()["Version"] << "], URI: " << client.getRequestMap()["Path"] << RESET << std::endl;
            openFile(client);
        }
    }
}

void Server::openFile(Client &client)
{
    int         fileFd;
    std::string file;
    ServerInfo& serverInfo = client.getServerInfo();
    bool        locationFound = false;

    file = client.getRequestMap().at("Path");

    for (const Location& location : serverInfo.getLocations())
    {
        // Check if the file path starts with the location path
        if (file.find(location.getPath()) == 0)
        {
            file = location.getRoot() + file.substr(location.getPath().length());
            // Next check is not complete.
            // We need to check auto-index
            // If auto-index == false, server location index?
            if (file.back() == '/' && location.getAutoIndex() == true)
            {
                client.setWriteBuffer(generateFolderContent(file));
                return;
            }

            // else if (file.back() == '/')  * check here if there is an index specified within the location block
                file += location.getIndex(); // Use the index if the path ends with "/"
            // else Error is found so set the Error status to 404
            locationFound = true;
            break;
        }
    }

    // If no matching location was found, use the server root
    // I'm not sure about this part yet, maybe we just serve an error page
    if (!locationFound)
    {
        if (file == "/")
            file += serverInfo.getIndex();
        file = serverInfo.getRoot() + file;
    }

    std::cout << "Constructed file path: " << file << std::endl;

    fileFd = open(file.c_str(), O_RDONLY);
    if (fileFd < 0)
    {
        client.setStatusCode(404); 
        // call function to open error file
    }
    else
    {
        // If file is successfully opened, add it to the poll loop
        client.setFileFd(fileFd);
        struct pollfd filePollFd;
        filePollFd.fd = fileFd;
        filePollFd.events = POLLIN;
        _pollFds.push_back(filePollFd);
    }
}

std:: string Server::generateFolderContent(std::string path)
{
    std::ostringstream  html;
    struct dirent       *entry;
    DIR                 *folder;
    
    html << "<!DOCTYPE html>";  // Include the DOCTYPE declaration
    html << "<html><head>";
    html << "<meta charset=\"UTF-8\">";  // Define the character set
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";  // Responsive design
    html << "<title>Index of " << path << "</title>";  // Set the title
    html << "</head><body>";
    html << "<h1>Index of " << path << "</h1>";
    html << "<ul>";

    if (folder = opendir(path.c_str()))
    {
        while (entry = readdir(folder))
        {
            std::string name = entry->d_name;
            
            if (name == "." || name == "..")
                continue;

            std::string fullPath = path + (path.back() == '/' ? "" : "/") + name;

            if (entry->d_type == DT_DIR)
                name += "/";
            
            html << "<li><a href=\"" << name << "\">" << name << "</a></li>";
        }
        closedir(folder);
    }
    else
        html << "<li>Unable to open directory</li>";
    
    html << "<ul>";
    html << "</body></hmtl>";

    return html.str();
}

// void Server::openFile(Client &client)
// {
//     int fileFd;
//     std::string file;

//     // if Error call new function that builds an error Path.
//     // When does the error get detected? if after the path building
//     // needs to be redone after trying to open the original file request

//     file = client.getRequestMap().at("Path");
//     if (file == "/")
//         file += client.getServerInfo().getIndex();
//     file = client.getServerInfo().getRoot() + file;

//     std::cout << "Constructed file path: " << file << std::endl;
//     fileFd = open(file.c_str(), O_RDONLY);
//     if (fileFd < 0)
//     //  build path to 404 file, open it and add fd to Poll loop
//         client.setStatusCode(404); 
//     else
//     {
//         client.setFileFd(fileFd);
//         struct pollfd filePollFd;
//         filePollFd.fd = fileFd;
//         filePollFd.events = POLLIN;
//         _pollFds.push_back(filePollFd);
//     }
// }

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
                for (auto& value : _pollFds)
                {
                    if (value.fd == it->second.getFd())
                        value.events = POLLOUT;
                }
                int i = 0;
                for (auto& value : _pollFds)
                {
                    if (value.fd == it->second.getFileFd())
                        _pollFds.erase(_pollFds.begin() + i);
                    i++;
                }
            }
        }
    }
}

void Server::sendClientData(size_t index)
{
    Client& client = getClient(_pollFds[index].fd);

    // if(!client.getResponseStatus())
    //     handleClientRequest(client);  
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

void Server::closeConnection(size_t index)
{
    int fd = _pollFds[index].fd;

    close(fd);
    _pollFds.erase(_pollFds.begin() + index);
    removeClient(fd);
}

void Server::removeClient(int fd)
{
    _clients.erase(fd);
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



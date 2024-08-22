#include "../includes/Server.hpp"
#include "../includes/Config.hpp"

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

// void Server::createServerInstances()
// {
//     ServerBlock server1;
//     server1.setPort(4040);
//     server1.setIndex("index.html");
//     server1.setRoot("./html");
//     server1.setHost("127.0.0.1");

//     ServerBlock server2;
//     server2.setPort(8080);
//     server2.setIndex("index.html");
//     server2.setRoot("./html1");
//     server2.setHost("127.0.0.1");

//     _servers.push_back(server1);
//     _servers.push_back(server2);
// }

/*********************************************************************/

// Adds a new server instance to the server list

void    Server::addServer(const ServerBlock& ServerBlock)
{
    _servers.push_back(ServerBlock);
}

// Creates server sockets and adds them to the poll loop

void Server::createServerSockets()
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
        
        addClient(newSocket, getServerBlockByFd(serverSocket));
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
            if (client.getState() == ERROR){
                client.createResponse();
                return;
            }
            std::cout << GREEN << "Request Received from socket " << _pollFds[index].fd << ", method: [" << client.getRequestMap()["Method"] << "], version: [" << client.getRequestMap()["Version"] << "], URI: " << client.getRequestMap()["Path"] << RESET << std::endl;
            if (_cgi.checkIfCGI(client) == true){
                _cgi.runCGI(*this, client);
                _pollFds[index].events = POLLOUT; // CGI finished, so POLLOUT can be set
            }
            else{
                openFile(client);
            }
            if (client.getState() == ERROR)
            {
                client.createResponse();
                _pollFds[index].events = POLLOUT;
                return ;
            }
        }
    }
}

void Server::openFile(Client &client)
{
    int                     fileFd;
    std::string             file;
    ServerBlock&            serverBlock = client.getServerBlock();
    std::vector<Location>   matchingLocations;
    bool                    locationFound = false;

    file = client.getRequestMap().at("Path");

    for (const Location& location : serverBlock.getLocations())
    {
        if (file.find(location.getPath()) == 0)
            matchingLocations.push_back(location);
    }

    std::sort(matchingLocations.begin(), matchingLocations.end(), sortLocations);

    for (const Location& location : matchingLocations)
    {
        std::string locationRoot = location.getRoot();
        std::string locationPath = location.getPath();

        if (locationRoot.empty())
            locationRoot = serverBlock.getRoot();
        
        if (!locationRoot.empty() && locationRoot.back() == '/')
            locationRoot.pop_back();

        std::string fileName = file.substr(locationPath.length());
        if (!fileName.empty() && fileName.front() == '/')
            fileName.erase(fileName.begin());

        file = locationRoot + "/" + fileName;

        if (file.back() == '/' && location.getAutoindex())
        {
            client.setWriteBuffer(generateFolderContent(file));
            return;
        }
        if (file.back() == '/' && !location.getIndex().empty())
            file += location.getIndex();

        locationFound = true;
        break;
    }

    // If no matching location was found, use the server root
    if (!locationFound)
    {
        if (file.back() == '/')
            file += serverBlock.getIndex();

        std::string serverRoot = serverBlock.getRoot();
        if (!serverRoot.empty() && serverRoot.back() == '/')
            serverRoot.pop_back();

        if (file.front() == '/')
            file = serverRoot + file;
        else
            file = serverRoot + "/" + file;
    }

    // Normalize path: Replace multiple slashes with a single slash
    file = std::regex_replace(file, std::regex("//+"), "/");

    std::cout << "Constructed file path: " << file << std::endl;

    fileFd = open(file.c_str(), O_RDONLY);
    if (fileFd < 0)
    {
        client.setStatusCode(404); 
        // Call function to open error file (not implemented here)
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

// void Server::openFile(Client &client)
// {
//     int         fileFd;
//     std::string file;
//     ServerBlock& serverBlock = client.getServerBlock();
//     bool        locationFound = false;

//     file = client.getRequestMap().at("Path");

//     std::cout << RED << serverBlock.getLocations().size() << RESET << std::endl;

//     for (const Location& location : serverBlock.getLocations())
//     {
//         // Check if the file path starts with the location path
//         if (file.find(location.getPath()) == 0)
//         {
//             std::string locationRoot = location.getRoot();
//             std::string locationPath = location.getPath();
//             std::cout << RED << locationRoot << "\n" << locationPath << RESET << std::endl;

            
//             // Ensure no trailing slash on root before appending the file
//             if (!locationRoot.empty() && locationRoot.back() == '/')
//                 locationRoot.pop_back();
            
//             // Avoid double slash by removing the first slash if present
//             if (!locationPath.empty() && locationPath.back() == '/' && file[locationPath.length()] == '/')
//                 file = locationRoot + file.substr(locationPath.length() + 1);
//             else
//             {
//                file = locationRoot + file.substr(locationPath.length());
//                 std::cout << RED << "TEST" << RESET << std::endl;
//             }

//             if (file.back() == '/' && location.getAutoindex() == true)
//             {
//                 client.setWriteBuffer(generateFolderContent(file));
//                 return;
//             }
//             if (file.back() == '/' && !location.getIndex().empty())
//                 file += location.getIndex();

//             locationFound = true;
//             break;
//         }
//     }

//     // If no matching location was found, use the server root
//     // ************* We probably just want to serve a 404 error here ****************
//     if (!locationFound)
//     {
//         if (file.back() == '/')
//             file += serverBlock.getIndex();

//         std::string serverRoot = serverBlock.getRoot();
//         if (!serverRoot.empty() && serverRoot.back() == '/')
//             serverRoot.pop_back();

//         if (file.front() == '/')
//             file = serverRoot + file;
//         else
//             file = serverRoot + "/" + file;
//     }

//     // Normalize path: Replace multiple slashes with a single slash
//     // not sure if this is still necessary
//     file = std::regex_replace(file, std::regex("//+"), "/");

//     std::cout << "Constructed file path: " << file << std::endl;

//     fileFd = open(file.c_str(), O_RDONLY);
//     if (fileFd < 0)
//     {
//         client.setStatusCode(404); 
//         // call function to open error file
//     }
//     else
//     {
//         // If file is successfully opened, add it to the poll loop
//         client.setFileFd(fileFd);
//         struct pollfd filePollFd;
//         filePollFd.fd = fileFd;
//         filePollFd.events = POLLIN;
//         _pollFds.push_back(filePollFd);
//     }
// }

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

    if (folder == opendir(path.c_str()))
    {
        while (entry == readdir(folder))
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
                removePollFd(it->second.getFileFd());
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
            client.resetClientData(); // Resetting all data of client. Right location?
            closeConnection(index);
        }
    }
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
    for (auto& ServerBlock : _servers)
    {
        if (ServerBlock.getServerFd() != -1)
            close(ServerBlock.getServerFd());
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
// Retrieve the ServerBlock object corresponding to a given file descriptor

ServerBlock& Server::getServerBlockByFd(int fd)
{
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        if (_servers[i].getServerFd() == fd)
            return _servers[i];
    }
    throw std::runtime_error("ServerBlock with the given fd not found");
}

void Server::addClient(int fd, ServerBlock& ServerBlock)
{
    _clients.emplace(fd, Client(fd, ServerBlock));
}

Client& Server::getClient(int fd)
{
    return _clients.at(fd);
}

void            Server::setServer(std::vector<ServerBlock> serverBlocks)
{
    _servers = serverBlocks;
}

bool    sortLocations(const Location& a, const Location& b)
{
    return (a.getPath().length()> b.getPath().length());
}
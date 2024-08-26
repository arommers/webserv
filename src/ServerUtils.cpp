#include "../includes/Server.hpp"
#include "../includes/Config.hpp"
#include <thread>

void Server::openFile(Client &client)
{
    std::string file = client.getRequestMap().at("Path");
    std::string method = client.getRequestMap().at("Method");
    ServerBlock& serverBlock = client.getServerBlock();

    std::vector<Location> matchingLocations = findMatchingLocations(file, serverBlock);
    if (matchingLocations.empty())
    {
        client.setStatusCode(404); 
        return;
    }

    const Location& location = matchingLocations[0];
    
    if (!checkAllowedMethod(location, method))
    {
        client.setStatusCode(405);
        return;
    }

    std::string resolvedFile = resolveFilePath(file, location, serverBlock);

    if (!checkFileAccessRights(resolvedFile))
    {
        client.setStatusCode(403);
        return;
    }

    if (handleDirectoryRequest(resolvedFile, location, client))
        return;

    openRequestedFile(resolvedFile, client);
}

std::vector<Location> Server::findMatchingLocations(const std::string& file, ServerBlock& serverBlock)
{
    std::vector<Location> matchingLocations;
    for (const Location& location : serverBlock.getLocations())
    {
        if (file.find(location.getPath()) == 0)
            matchingLocations.push_back(location);
    }

    std::sort(matchingLocations.begin(), matchingLocations.end(), sortLocations);
    return matchingLocations;
}

bool Server::checkAllowedMethod(const Location& location, const std::string& method)
{
    const std::vector<std::string>& allowedMethods = location.getAllowedMethods();
    return std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end();
}

std::string Server::resolveFilePath(const std::string& file, const Location& location, ServerBlock& serverBlock)
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

    std::string resolvedFile = locationRoot + "/" + fileName;

    // Normalize path: Replace multiple slashes with a single slash
    resolvedFile = std::regex_replace(resolvedFile, std::regex("//+"), "/");
    return resolvedFile;
}

bool Server::handleDirectoryRequest(std::string& file, const Location& location, Client& client)
{
    struct stat fileStat;
    if (stat(file.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
    {
        if (location.getAutoindex())
        {
            
            if (file.back() != '/')
                file += "/";

            client.setFileBuffer(generateFolderContent(file));
            client.setState(RESPONSE);
            return true;
        }
        else if (!location.getIndex().empty())
            file += "/" + location.getIndex();
        else
        {
            client.setStatusCode(403);
            return true;
        }
    }
    return false;
}

void Server::openRequestedFile(const std::string& file, Client& client)
{
    int fileFd = open(file.c_str(), O_RDONLY);
    if (fileFd < 0)
    {
        client.setStatusCode(404);
        return;
    }
    else
    {
        client.setReadWriteFd(fileFd); 
        struct pollfd filePollFd;
        filePollFd.fd = fileFd;
        filePollFd.events = POLLIN;
        _pollFds.push_back(filePollFd);
        client.setState(READING);
    }
}

std::string Server::generateFolderContent(std::string path)
{
    std::ostringstream  html;
    struct dirent       *entry;
    DIR                 *folder = opendir(path.c_str());

    if (!folder)
    {
        std::cerr << "Failed to open directory: " << path << std::endl;
        html << "<li>Unable to open directory</li>";
        return html.str();
    }

    html << "<!DOCTYPE html>";
    html << "<html><head>";
    html << "<meta charset=\"UTF-8\">";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    html << "<title>Index of " << path << "</title>";
    html << "</head><body>";
    html << "<h1>Index of " << path << "</h1>";
    html << "<ul>";

    while ((entry = readdir(folder)) != nullptr)
    {
        std::string name = entry->d_name;

        std::cout << "Processing entry: " << name << std::endl;  // Debugging statement

        if (name == "." || name == "..")
            continue;

        std::string fullPath = path + name;

        if (entry->d_type == DT_DIR)
            name += "/";

        html << "<li><a href=\"" << name << "\">" << name << "</a></li>";
    }

    closedir(folder);
    
    html << "</ul>";
    html << "</body></html>";

    return html.str();
}

// void Server::checkTimeout(int time)
// {
//     std::time_t currentTime = std::time(nullptr);

//     for (size_t i = 0 + getServer().size(); i < _pollFds.size(); ++i)
//     {
//         std::cout << RED << "Test" << RESET << std::endl;
        
//         int fd = _pollFds[i].fd;

//         std::cout << RED << fd << RESET << std::endl;

//         if (_clients.count(fd))
//         {
//             Client& client = getClient(fd);
//             std::cout << "Client Time : " << client.getTime() << ", Current Time: " << currentTime << std::endl;

//             if (difftime(currentTime, client.getTime()) > time)
//             {
//                 std::cout << YELLOW << "Connection timeout, closing socket fd: " << fd << RESET << std::endl;
//                 closeConnection(i);
//                 --i;
//             }
//         }
//     }
// }

// void    Server::checkTimeout(int time)
// {
//     std::time_t currentTime = std::time(nullptr);

//     for (size_t i = 1; i < _pollFds.size(); ++i)
//     {
//         Client& client = getClient(_pollFds[i].fd);

//         if (difftime(currentTime, client.getTime()) > time)
//         {
//             std::cout << YELLOW << "Connection timeout, closing socket fd: " << _pollFds[i].fd << RESET << std::endl;
//             closeConnection(i);
//             --i;
//         }
//     }
// }

bool Server::checkFileAccessRights(const std::string& path)
{
    return (access(path.c_str(), R_OK) == 0) ? true : false;
}

bool    sortLocations(const Location& a, const Location& b)
{
    return (a.getPath().length()> b.getPath().length());
}
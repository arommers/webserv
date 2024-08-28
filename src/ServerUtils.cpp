#include "../includes/Server.hpp"
#include "../includes/Config.hpp"
#include <thread>

void Server::openFile(Client &client)
{
    std::string file = client.getRequestMap().at("Path");
    std::string method = client.getRequestMap().at("Method");
    ServerBlock& serverBlock = client.getServerBlock();
    std::string resolvedFile;

    std::vector<Location> matchingLocations = findMatchingLocations(file, serverBlock);
    if (matchingLocations.empty())
    {
        resolvedFile = serverBlock.getRoot() + serverBlock.getIndex();
        // client.setStatusCode(404); 
        // return;
    }
    else
    {
        const Location& location = matchingLocations[0];
        
        if (!checkAllowedMethod(location, method))
        {
            client.setStatusCode(405);
            return;
        }

        resolvedFile = resolveFilePath(file, location, serverBlock);

        if (!checkFileExists(resolvedFile))
        {
            client.setStatusCode(404);
            return;
        }

        if (!checkFileAccessRights(resolvedFile))
        {
            client.setStatusCode(403);
            return;
        }

        if (handleDirectoryRequest(resolvedFile, location, client))
            return;
    }

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

    resolvedFile = std::regex_replace(resolvedFile, std::regex("//+"), "/");
    return resolvedFile;
}

bool Server::handleDirectoryRequest(std::string& file, const Location& location, Client& client)
{
    struct stat fileStat;
    
    // Check if the requested path is a directory
    if (stat(file.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
    {
        // If an index is specified in the location block
        if (!location.getIndex().empty())
        {
            // Append the index file to the directory path
            std::string indexPath = file + "/" + location.getIndex();

            // Check if the index file exists and is a regular file
            if (stat(indexPath.c_str(), &fileStat) == 0 && S_ISREG(fileStat.st_mode))
            {
                file = indexPath; // Update file path to the index file
                return false; // Continue with the request, which will serve the index file
            }
        }

        // If autoindex is on and no index file was found
        if (location.getAutoindex())
        {
            if (file.back() != '/')
                file += "/";

            client.setFileBuffer(generateFolderContent(file));
            client.setState(RESPONSE);
            return true;
        }
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

bool Server::checkFileExists(const std::string& path)
{
    return (access(path.c_str(), F_OK) == 0) ? true : false;
}

bool Server::checkFileAccessRights(const std::string& path)
{
    return (access(path.c_str(), R_OK) == 0) ? true : false;
}

bool    sortLocations(const Location& a, const Location& b)
{
    return (a.getPath().length()> b.getPath().length());
}
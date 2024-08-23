#include "../includes/Cgi.hpp"
#include "../includes/Server.hpp"


Cgi::Cgi()
{

}

Cgi::Cgi(const Cgi& rhs)
{

}

Cgi& Cgi::operator=(const Cgi& rhs)
{
    return (*this);
}

Cgi::~Cgi()
{

}

bool Cgi::checkIfCGI( Client &client )
{
    if (client.getRequestMap().at("Path").find("/cgi-bin/") != std::string::npos){
        return (true);
    }
    return (false);
}


void Cgi::runCGI( Server& server, Client& client)
{
    if (client.getState() == START)
    {
        if (client.getRequestMap().at("Method") == "POST"){    
            createPipe(server, client, client.getRequestPipe());
            client.setWriteBuffer(client.getRequestMap().at("Body"));
            client.setReadWriteFd(client.getRequestPipe()[1]);
            server.addPollFd(client.getRequestPipe()[1], POLLOUT);
            client.setState(WRITING);
        }
        else
            client.setState(READY);
        createPipe(server, client, client.getResponsePipe());
        createFork(server, client);

    }
    if (client.getState() == READY){
        waitpid(_pid, nullptr, 0); // Is this correct? Will the server hang if a child is hanging?
        client.setReadWriteFd(client.getResponsePipe()[0]);
        close(client.getResponsePipe()[1]);
        if (client.getRequestMap().at("Method") == "POST")
            close(client.getRequestPipe()[0]);
        server.addPollFd(client.getResponsePipe()[0], POLLIN);
        client.setState(READING);
    }
}



char** Cgi::createEnv(Server& server, Client& client)
{
    std::vector<std::string>    env_vec;

    env_vec.push_back("REQUEST_METHOD=" + client.getRequestMap().at("Method"));
    if (client.getRequestMap().count("Body"))
        env_vec.push_back("CONTENT_LENGTH=" + std::to_string(client.getRequestMap().at("Body").length() - 2));
    if (client.getRequestMap().count("Content-Type"))
        env_vec.push_back("CONTENT_TYPE=" + client.getRequestMap().at("Content-Type"));
    env_vec.push_back("BUFFER_SIZE="+std::to_string(BUFFER_SIZE));
    char** env = new char*[env_vec.size() + 1];
    for (int i = 0; i < env_vec.size(); i++){
        env[i] = new char[env_vec[i].size() + 1];
        std::strcpy(env[i], env_vec[i].c_str());
    }
    env[env_vec.size()] = nullptr;
    return (env);
}

void Cgi::createPipe(Server& server, Client& client, int* fdPipe)
{
    if (pipe(fdPipe) == -1){
        perror("pipe");
        exit(1);
    }
}

void Cgi::createFork(Server& server, Client& client)
{
    _pid = fork();
    if (_pid == -1){
        perror("fork");
        exit (1);
    }
    else if (_pid == 0) // Entering child process
    {
        launchScript(server, client);
    }
}

void Cgi::redirectToPipes(Server& server, Client& client)
{
    if (client.getRequestMap().at("Method") == "POST"){
        close(client.getRequestPipe()[1]);
        if (dup2(client.getRequestPipe()[0], STDIN_FILENO) == -1){
            perror("dup2");
            exit(1);
        }
        close(client.getRequestPipe()[0]);

    }
    close(client.getResponsePipe()[0]);
    if (dup2(client.getResponsePipe()[1], STDOUT_FILENO) == -1){
        perror("dup2");
        exit(1);
    }
    close(client.getResponsePipe()[1]);

}

void Cgi::launchScript(Server& server, Client& client)
{
    std::string path = findPath(server, client);
    char * pathArray[] = {const_cast<char *>(path.c_str()), nullptr};
    char** env = createEnv(server, client);
    redirectToPipes(server, client);
    execve(pathArray[0], pathArray, env);
    perror("execve");
    exit(1);
}

std::string Cgi::findPath(Server& server, Client& client)
{
    std::string             path;
    ServerBlock&            serverBlock = client.getServerBlock();
    std::vector<Location>   matchingLocations;
    bool                    locationFound = false;

    path = client.getRequestMap().at("Path");

    for (const Location& location : serverBlock.getLocations())
    {
        if (path.find(location.getPath()) == 0)
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

        std::string fileName = path.substr(locationPath.length());
        if (!fileName.empty() && fileName.front() == '/')
            fileName.erase(fileName.begin());

        path = locationRoot + "/" + fileName;

        locationFound = true;
        break;
    }
    return (path);
}
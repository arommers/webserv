#include "../includes/Cgi.hpp"

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
    std::cout << "Test!\n";
    if (client.getRequestMap().at("Path").find("/cgi-bin/") != std::string::npos){
        std::cout << "Test: " << client.getRequestMap().at("Path") << std::endl;
        return (true);
    }
    return (false);
}


void Cgi::runCGI( Server& server, Client& client)
{
    std::cout << "In runCGI\n";
    createFork(server, client);
}



char** Cgi::createEnv(Server& server, Client& client)
{
    std::vector<std::string>    env_vec;

    // client.printRequestMap();
    env_vec.push_back("METHOD=" + client.getRequestMap().at("Method"));
    env_vec.push_back("QUERY_STRING=test"); // ADD!
    env_vec.push_back("QUERY_LENGTH=4");
    env_vec.push_back("SERVER_NAME=localhost");
    env_vec.push_back("SERVER_PORT=8080");
    env_vec.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env_vec.push_back("REMOTE_ADDR=127.0.0.1");
    env_vec.push_back("BODY=" + client.getRequestMap().at("Body"));

    char** env = new char*[env_vec.size() + 1];
    for (int i = 0; i < env_vec.size(); i++){
        env[i] = new char[env_vec[i].size() + 1];
        std::strcpy(env[i], env_vec[i].c_str());
    }
    env[env_vec.size()] = nullptr;
    return (env);
}

void Cgi::createFork(Server& server, Client& client)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit (1);
    }
    else if (pid == 0)
    {
        launchScript(server, client);
    }
    else
        _childForks.push_back(pid);
}

void Cgi::launchScript(Server& server, Client& client)
{
    std::string path = "." + client.getRequestMap().at("Path");
    char * pathArray[] = {const_cast<char *>(path.c_str()), nullptr};

    execve(pathArray[0], pathArray, createEnv(server, client));
    perror("execve");
    exit(1);
}
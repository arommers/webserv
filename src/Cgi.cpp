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
    // client.printRequestMap();

    if (client.getState() == START)
    {
        if (client.getRequestMap().at("Method") == "POST"){    
            createPipe(server, client, client.getRequestPipe());
            writeBodyToPipe(server, client);
        }
        createPipe(server, client, client.getReponsePipe());

        client.setWriteBuffer(client.getRequestMap().at("Body"));
        client.setReadWriteFd(client.getRequestPipe()[1]);
        server.addPollFd(client.getRequestPipe()[1], POLLOUT); 
        

        return ;
    }
    else if ((client.getState() == READY))
        createFork(server, client);
}



char** Cgi::createEnv(Server& server, Client& client)
{
    std::vector<std::string>    env_vec;

    env_vec.push_back("REQUEST_METHOD=" + client.getRequestMap().at("Method"));
    if (client.getRequestMap().count("Body"))
        env_vec.push_back("CONTENT_LENGTH=" + std::to_string(client.getRequestMap().at("Body").length() - 2));
    if (client.getRequestMap().count("Content-Type"))
        env_vec.push_back("CONTENT_TYPE=" + client.getRequestMap().at("Content-Type"));
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
    struct pollfd pipeFdRead;
    struct pollfd pipeFdWrite;

    if (pipe(fdPipe) == -1){
        perror("pipe");
        exit(1);
    }
}

void Cgi::readClosePipes(Server& server, Client& client)
{
    int     bufferSize = 2000;
    char    buffer[bufferSize];

    size_t bytesRead = read(client.getReponsePipe()[0], buffer, bufferSize - 1);
    if (bytesRead == -1){
        perror("read");
        exit(1);
    }
    buffer[bytesRead] = '\0';
    client.setFileBuffer(buffer);
    client.createResponse();
    
    // server.removePollFd(client.getRequestPipe()[0]);
    // server.removePollFd(client.getRequestPipe()[1]);
    // server.removePollFd(client.getReponsePipe()[0]);
    // server.removePollFd(client.getReponsePipe()[1]);
    // close (client.getRequestPipe()[0]);
    // close (client.getRequestPipe()[1]);
    // close (client.getReponsePipe()[0]);
    // close (client.getReponsePipe()[1]);
}

void Cgi::writeBodyToPipe(Server& server, Client& client)
{
    int             writeFd;
    struct pollfd   pollFd;

    writeFd = write(client.getRequestPipe()[1], client.getRequestMap().at("Body").c_str(), client.getRequestMap().at("Body").length());
    if (writeFd < 0){
        perror("write");
        exit(1);
    }
}

void Cgi::createFork(Server& server, Client& client)
{
    pid_t   pid;

   
    pid = fork();
    if (pid == -1){
        perror("fork");
        exit (1);
    }
    else if (pid == 0) // Entering child process
    {
        launchScript(server, client);
    }
    else // In parent
    {
        _childForks.push_back(pid);
        waitpid(pid, nullptr, 0); // Is this correct? Will the server hang if a child is hanging?
        readClosePipes(server, client);
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
    }
    close(client.getReponsePipe()[0]);
    if (dup2(client.getReponsePipe()[1], STDOUT_FILENO) == -1){
        perror("dup2");
        exit(1);
    }
}

void Cgi::launchScript(Server& server, Client& client)
{
    std::string path = "." + client.getRequestMap().at("Path");
    char * pathArray[] = {const_cast<char *>(path.c_str()), nullptr};
    write(2, "Hier!\n", 6);

    char** env = createEnv(server, client);


    redirectToPipes(server, client);
    execve(pathArray[0], pathArray, env);
    perror("execve");
    exit(1);
}
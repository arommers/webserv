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
            std::cout << "Requestpipe read+write: " << client.getRequestPipe()[0] << " " << client.getRequestPipe()[1] << std::endl;
            client.setWriteBuffer(client.getRequestMap().at("Body"));
            client.setReadWriteFd(client.getRequestPipe()[1]);
            std::cout << "Set readwritefd to: " << client.getReadWriteFd() << std::endl;
            std::cout << "Size of poll before: " << server.getPollFds().size() << std::endl;
            // server.addPollFd(client.getRequestPipe()[1], POLLOUT);
            struct pollfd pollFd;
            pollFd.fd = client.getRequestPipe()[1];
            pollFd.events = POLLOUT;
            server.addPollFd(pollFd);
            std::cout << "Size of poll after : " << server.getPollFds().size() << std::endl;
            client.setState(WRITING);
            return ;
        }
        else
            client.setState(READY); 

        return ;
    }
    else if ((client.getState() == READY)){
        std::cout << "Creating reponse pipe!!!!!!!!!!!!\n";
        char buf[5000];
        int readbytes = read(client.getRequestPipe()[0], buf, 5000);
        std::cout << "In Request pipe: " << client.getRequestPipe()[0] << std::endl << buf << std::endl;
        createPipe(server, client, client.getReponsePipe());
        createFork(server, client);
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
    // std::cout << "Buffer: " << buffer << std::endl;
    // client.createResponse();
    
    // server.removePollFd(client.getRequestPipe()[0]);
    // server.removePollFd(client.getRequestPipe()[1]);
    // server.removePollFd(client.getReponsePipe()[0]);
    // server.removePollFd(client.getReponsePipe()[1]);
    // close (client.getRequestPipe()[0]);
    // close (client.getRequestPipe()[1]);
    // close (client.getReponsePipe()[0]);
    // close (client.getReponsePipe()[1]);
}

void Cgi::createFork(Server& server, Client& client)
{
    pid_t   pid;

    std::cout << "At CreateFork\n";
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
        client.setState(RESPONSE);
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
#include "../includes/Cgi.hpp"
#include "../includes/Server.hpp"

// --- Constructors/ Deconstructor ---
Cgi::Cgi() {}
Cgi::~Cgi() {}

// --- CGI Functions ---
bool	Cgi::checkIfCGI( Client &client )
{
	if (client.getRequestMap().at("Path").find("/cgi-bin/") != std::string::npos){
		if (client.getRequestMap().at("Path").back() != '/')
		return (true);
	}
	return (false);
}

void	Cgi::runCGI( Server& server, Client& client)
{
	int status;

	if (client.getState() == START)
	{
		if (client.getRequestMap().at("Method") == "POST"){    
			createPipe(client, client.getRequestPipe());
			if (client.getState() == ERROR)
				return ;
			client.setWriteBuffer(client.getRequestMap().at("Body"));
			client.setReadWriteFd(client.getRequestPipe()[1]);
			server.addPollFd(client.getRequestPipe()[1], POLLOUT);
			client.setState(WRITING);
		}
		else
			client.setState(READY);
		createPipe(client, client.getResponsePipe());
		if (client.getState() == ERROR)
				return ;
		createFork(client);
	}
	else if (client.getState() == READY){
		int result = waitpid(_pid, &status, WNOHANG);
		if (result == 0){
			usleep(100000); // Sleep for 100ms
		}
		else if (WIFEXITED(status)) {
			int exit_status = WEXITSTATUS(status);
			if (exit_status != EXIT_SUCCESS){
				client.setStatusCode(500);
				closeAllPipes(client);
				return ;
			}
			client.setReadWriteFd(client.getResponsePipe()[0]);
			close(client.getResponsePipe()[1]);
			if (client.getRequestMap().at("Method") == "POST")
				close(client.getRequestPipe()[0]);
			if(client.getState() != ERROR){
				server.addPollFd(client.getResponsePipe()[0], POLLIN);
				client.setState(READING);
			}
		}
	}
}

char**	Cgi::createEnv(Client& client)
{
	std::vector<std::string>    env_vec;

	env_vec.push_back("REQUEST_METHOD=" + client.getRequestMap().at("Method"));
	if (client.getRequestMap().count("Body"))
		env_vec.push_back("CONTENT_LENGTH=" + std::to_string(client.getRequestMap().at("Body").length() - 2));
	if (client.getRequestMap().count("Content-Type"))
		env_vec.push_back("CONTENT_TYPE=" + client.getRequestMap().at("Content-Type"));
	env_vec.push_back("BUFFER_SIZE="+std::to_string(BUFFER_SIZE));
	char** env = new char*[env_vec.size() + 1];
	for (auto i = 0u; i < env_vec.size(); ++i){
		env[i] = new char[env_vec[i].size() + 1];
		std::strcpy(env[i], env_vec[i].c_str());
	}
	env[env_vec.size()] = nullptr;
	return (env);
}

void	Cgi::createPipe(Client& client, int* fdPipe)
{
	if (pipe(fdPipe) == -1){
		client.setStatusCode(500);
	}
}

void	Cgi::createFork(Client& client)
{
	_path = findPath(client);
	if (_path.empty())
	{
		client.setStatusCode(404);
		closeAllPipes(client);
		return ;
	}
	_pid = fork();
	if (_pid == -1){
		client.setStatusCode(500);
	}
	else if (_pid == 0) // Entering child process
	{
		launchScript(client);
	}
}

void	Cgi::redirectToPipes(Client& client)
{
	if (client.getRequestMap().at("Method") == "POST"){
		close(client.getRequestPipe()[1]);
		if (dup2(client.getRequestPipe()[0], STDIN_FILENO) == -1){
			client.setStatusCode(500);
			return ;
		}
		close(client.getRequestPipe()[0]);

	}
	close(client.getResponsePipe()[0]);
	if (dup2(client.getResponsePipe()[1], STDOUT_FILENO) == -1){
		client.setStatusCode(500);
		return ;
	}
	close(client.getResponsePipe()[1]);
}

void	Cgi::launchScript(Client& client)
{
	char * pathArray[] = {const_cast<char *>(_path.c_str()), nullptr};
	char** env = createEnv(client);
	redirectToPipes(client);
	if (client.getState() == ERROR)
		exit(EXIT_FAILURE);
	execve(pathArray[0], pathArray, env);
	exit(EXIT_FAILURE);
}

std::string	Cgi::findPath(Client& client)
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
	std::ifstream file(path);
	if (!file)
		return ("");
	return (path);
}

bool Cgi::isPipeEmpty(int fd) {
	int bytesAvailable = 0;

	if (ioctl(fd, FIONREAD, &bytesAvailable) == -1)
	{
		// Handle error
		std::cerr << "ioctl failed: " << strerror(errno) << std::endl;
		return true;  // or handle error accordingly
	}

	return bytesAvailable == 0;
}

void Cgi::closeAllPipes(Client& client)
{
	close(client.getResponsePipe()[1]);
	close(client.getResponsePipe()[0]);
	if (client.getRequestMap().at("Method") == "POST"){
		close(client.getRequestPipe()[0]);
		close(client.getRequestPipe()[1]);
	}					
}

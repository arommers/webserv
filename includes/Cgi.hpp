#pragma once

# include <iostream>
# include <cstring>
# include <vector>
# include <map>
# include <sys/wait.h>
# include <fcntl.h>
# include <sys/ioctl.h>
# include <chrono>

class Server;
class Client;

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

class Cgi
{
	private:
		pid_t		_pid;
		std::string	_path;
		TimePoint	_lastActivity;
		int			_responsePipe[2];
		int			_requestPipe[2];
	public:
		Cgi();
		~Cgi();
		
		bool		checkIfCGI(std::string path);
		void		runCGI( Server& server, Client& client);
		char**		createEnv(Client& client );
		void		createPipe(Client& client, int* fdPipe);
		void		createFork(Client& client);
		void		launchScript(Client& client);
		void		redirectToPipes(Client& client);
		std::string	findPath(Client& client);
		bool 		isPipeEmpty(int fd); // Remove?
		void		closeAllPipes(Client& client);
		int*		getResponsePipe();
		int*		getRequestPipe();
};
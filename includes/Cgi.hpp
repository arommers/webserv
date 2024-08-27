#pragma once

#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "../includes/Client.hpp"

class Server;

class Cgi
{
	private:
		pid_t		_pid;
		std::string	_path;
	public:
		Cgi();
		~Cgi();
		
		bool		checkIfCGI( Client& client );
		void		runCGI( Server& server, Client& client);
		char**		createEnv(Server& server, Client& client );
		void		createPipe(Server& server, Client& client, int* fdPipe);
		void		createFork(Server& server, Client& client);
		void		launchScript(Server& server, Client& client);
		void		redirectToPipes(Server& server, Client& client);
		std::string	findPath(Server& server, Client& client);
		bool 		isPipeEmpty(int fd);
		void		closeAllPipes(Client& client);
};
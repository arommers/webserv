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
		
		bool 		isPipeEmpty(int fd);
		bool		checkIfCGI( Client& client );
		void		runCGI( Server& server, Client& client);
		void		createPipe(Client& client, int* fdPipe);
		void		createFork(Client& client);
		void		launchScript(Client& client);
		void		redirectToPipes(Client& client);
		void		closeAllPipes(Client& client);
		char**		createEnv(Client& client );
		std::string	findPath(Client& client);
};
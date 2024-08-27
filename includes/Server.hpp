#pragma once

#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp" 
#include "../includes/ServerBlock.hpp"

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

class Cgi;
class Location;

class Server
{
	private:
		std::vector <ServerBlock>			_servers;
		std::vector<struct pollfd>			_pollFds;
		std::unordered_map<int, Client>		_clients;
		std::unordered_map<int, TimePoint>	_clientActivity;
		Cgi								_cgi;

	public:
		Server();
		~Server();

		void							addServer(const ServerBlock& serverInfo);
		void							createServerSockets();
		void							createPollLoop();
		void							acceptConnection(int serverSocket);
		void							handleClientData(size_t index);
		void							openFile(Client& client);
		void							handleFileRead(size_t index);
		void							sendClientData(size_t index);
		void							shutdownServer();
		void							closeConnection(size_t fd);
		void							removeClient(int fd);
		void							addClient(int fd, ServerBlock& serverInfo);
		void							removePollFd( int fd );
		void							setServer(std::vector<ServerBlock> serverBlocks);
		void							handleFdWrite(size_t index);
		void							addPollFd( int fd, short int events );
		void							addFileToPoll( Client& client, std::string file );
		void							handleDeleteRequest(Client& client);
		void							removePipe( size_t index );
		Client&							getClient(int fd);
		std::string						generateFolderContent(std::string path);
		ServerBlock&					getServerBlockByFd(int fd);
		std::vector<struct pollfd>		getPollFds();
		Location						getLocationForRequest(const std::string &uri, ServerBlock &serverBlock);
		void							updateClientActivity(int fd);
		void 							checkClientActivity();
};

bool	sortLocations(const Location& a, const Location& b);


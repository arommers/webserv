#pragma once

#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp" 
#include "../includes/ServerBlock.hpp"

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

class Cgi;
class Location;
class ServerBlock;

class Server
{
	private:
		std::vector <ServerBlock>			_servers;
		std::vector<struct pollfd>			_pollFds;
		std::unordered_map<int, Client>		_clients;
		std::unordered_map<int, TimePoint>	_clientActivity;
		Cgi									_cgi;

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
		void							openRequestedFile(const std::string& file, Client& client);
		void							updateClientActivity(int fd);
		void 							checkClientActivity();
		bool							handleDirectoryRequest(std::string& file, const Location& location, Client& client);
		bool							checkAllowedMethod(const Location& location, const std::string& method);
		bool							checkFileAccessRights(const std::string& path);
		Client&							getClient(int fd);
		std::string						generateFolderContent(std::string path);
		std::string						resolveFilePath(const std::string& file, const Location& location, ServerBlock& serverBlock);
		ServerBlock&					getServerBlockByFd(int fd);
		std::vector<struct pollfd>		getPollFds();
		std::vector<Location>			findMatchingLocations(const std::string& file, ServerBlock& serverBlock);
		ServerBlock&					getServerBlockForHost(const std::string& hostHeader);
		Location						getLocationForRequest(const std::string &uri, ServerBlock &serverBlock);
};

bool	sortLocations(const Location& a, const Location& b);


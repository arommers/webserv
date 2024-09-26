#pragma once

#include <iostream>       // Input and output through streams
#include <arpa/inet.h>    // Provides functions for manipulating IP addresses (htonl, htons)
#include <sys/socket.h>   // Provides declarations for the socket API functions (socket, bind, listen, access)
#include <netinet/in.h>   // Constants and structures needed for internet domain addresses ( sockaddr_in AF_INET)
#include <poll.h>         // Required for struct pollfd and poll
#include <unordered_map>
#include <string>
#include <ctime>
#include <dirent.h>       // Provides functions for using DIR directory stream like opendir(), readdir()
#include "../includes/ServerBlock.hpp"
#include "../includes/Cgi.hpp"
#include "../includes/Parsing.hpp"
#include "../includes/Status.hpp"

enum    clientState
{
    PARSE = 0, // Parsing the request
    START = 1, // Parsing has finished
    READING = 2, // Reading from file/pipe
    WRITING = 3, // Writing to file or pipe
    ERROR = 4, // Some error occured
    READY = 5, // Reading/Writing is finished and the process can continue
    RESPONSE = 6, // Everything has been done and the reponse can be build
	SENDING = 7
};

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

#define TIMEOUT 30000 //Miliseconds
#define BUFFER_SIZE 1024

class Client : public Cgi , public Parsing , public Error
{
	private:
		int										_fd;
		int										_state = PARSE;
		int										_readWriteFd = -1;
		int										_statusCode = 0;
		ServerBlock&							_ServerBlock;

	public:
		Client(int fd, ServerBlock& ServerBlock);
		~Client();

		// Utils
		void									readNextChunk();
		void									writeNextChunk();
		void									resetClientData( void );

		// Getters
		int										getFd();
		int										getState();
		int										getReadWriteFd();
		int										getStatusCode();
		ServerBlock&							getServerBlock();

		// Setters
		void									setFd( int fd );
		void									setState ( const int state );
		void									setReadWriteFd(int fd);
		void									setStatusCode( const int statusCode );
};

std::string	trimWhiteSpace(std::string& string);
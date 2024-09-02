#pragma once

#include <ctime>
#include <map>
#include <iostream>       // Input and output through streams
#include <cstring>        // Manipulate C-style strings and arrays
#include <arpa/inet.h>    // Provides functions for manipulating IP addresses (htonl, htons)
#include <unistd.h>       // Provides access to the POSIX operating system API (close, read, write)
#include <sys/socket.h>   // Provides declarations for the socket API functions (socket, bind, listen, access)
#include <netinet/in.h>   // Constants and structures needed for internet domain addresses ( sockaddr_in AF_INET)
#include <poll.h>         // Required for struct pollfd and poll
#include <fcntl.h>
#include <vector>
#include <unordered_map>
#include <csignal>
#include <fstream>        // Temporary for testing  
#include <algorithm>
#include <string>
#include <sstream>
#include <ctime>
#include <regex>
#include <dirent.h>       // Provides functions for using DIR directory stream like opendir(), readdir()
#include "ServerBlock.hpp"
#include "../includes/Cgi.hpp" 


class Cgi;

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

// Pending config parsing these are set
#define PORT 4040
#define MAX_CLIENTS 100
#define TIMEOUT 2000 //Miliseconds
#define BUFFER_SIZE 1024

class Client
{
	private:
		size_t									_writePos = 0;
		int										_fd = -1;
		int										_state = PARSE;
		int										_readWriteFd = -1;
		int										_statusCode = 0;
		int										_responsePipe[2];
		int										_requestPipe[2];
		std::string								_readBuffer;
		std::string								_writeBuffer;
		std::string								_fileBuffer;
		std::vector<int>						_statusCheck = {400, 401, 404, 405, 500, 503, 504};
		std::map<std::string, std::string>		_requestMap;
		std::map<std::string, std::string>		_responseMap;
		static const std::map<int, std::string>	_ReasonPhraseMap;
		ServerBlock								_ServerBlock;
		Cgi										_cgi;


		void									isValidMethod( std::string method );
		void									isValidPath( std::string path );
		void									isValidVersion( std::string version );

	public:
		Client();
		Client(int fd, ServerBlock& ServerBlock);
		~Client();

		void									addToBuffer( std::string bufferNew );
		void									parseBuffer ( void );
		void									printRequestMap( void );
		void									createResponse ( void );
		bool									checkIfCGI();
		void									setFd( int fd );
		void									setState ( const int state );
		void									setWritePos( size_t pos );
		void									setFileBuffer(std::string buffer);
		void									setWriteBuffer( std::string buffer );
		void									setStatusCode( const int statusCode );
		// void									setServerBlock(ServerBlock* serverBlock);
		void									resetClientData( void );    
		void									readNextChunk();
		void									writeNextChunk();
		void									setReadWriteFd(int fd);
		bool									requestComplete();
		bool									detectError();
		int										getFd();
		int										getState();
		int										getReadWriteFd();
		int										getStatusCode();
		int*									getResponsePipe();
		int*									getRequestPipe();
		size_t									getWritePos();
		std::string								getReadBuffer();
		ServerBlock&							getServerBlock();
		std::string								getWriteBuffer();
		std::map<std::string, std::string>		getRequestMap( void );
		std::map<std::string, std::string>&		getResponseMap();
};

std::string	trimWhiteSpace(std::string& string);
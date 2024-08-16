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
#include <string> // Needed? cstring or string library
#include <sstream>
#include <regex>
#include <dirent.h>       // Provides functions for using DIR directory stream like opendir(), readdir()
#include "ServerBlock.hpp"



enum    clientState
{
    READY = 0, // Reading is finished and the request can be parsed
    READING = 1, // Reading HTTP request
    SENDING = 2, // Sending HTTP request
    ERROR = 3 // Some error occured 
};

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

//pending config parsing these are set

#define PORT 4040
#define MAX_CLIENTS 10
#define TIMEOUT 60
#define BUFFER_SIZE 1024

class Client
{
    private:
        int                                     _fd = -1;
        int                                     _state = -1;
        int                                     _fileFd = -1;
        std::string                             _readBuffer;
        std::string                             _writeBuffer;
        std::string                             _fileBuffer;
        size_t                                  _writePos = 0;
        std::map<std::string, std::string>      _requestMap;
        std::map<std::string, std::string>      _responseMap;
        static const std::map<int, std::string> _ErrorMap;
        int                                     _statusCode = 0;
        std::time_t                             _time = std::time(nullptr);
        int                                     _requestPipe[2];
        int                                     _responsePipe[2];
        
        bool                                    _responseReady = false;
        ServerBlock                              _ServerBlock;

        void    isValidMethod( std::string method );
        void    isValidPath( std::string path );
        void    isValidVersion( std::string version );

    public:
        Client();
        Client(int fd, ServerBlock& ServerBlock);
        // Client(const Client& rhs);
        // Client& operator=(const Client& rhs);
        ~Client();

        void                                    addToBuffer( std::string bufferNew );
        std::string                             getReadBuffer();
        void                                    parseBuffer ( void );
        void                                    printRequestMap( void );
        void                                    createResponse ( void );
        void                                    setFd( int fd );
        int                                     getFd();
        int                                     getState();
        void                                    setState ( const int state );
        size_t                                  getWritePos();
        void                                    setWritePos( size_t pos );
        std::string                             getWriteBuffer();
        std::string                             getFileBuffer();
        void                                    setFileBuffer(std::string buffer);
        void                                    setWriteBuffer( std::string buffer );
        void                                    setStatusCode( const int statusCode );
        std::map<std::string, std::string>      getRequestMap( void );
        bool                                    requestComplete();
        std::time_t                             getTime();
        void                                    updateTime();
        std::string                             createErrorResponse( void );
        void                                    resetClientData( void );
        void                                    runCGI( void );        
        void                                    readNextChunk();
        bool                                    getResponseStatus();
        void                                    setFileFd(int fd);
        int                                     getFileFd();
        bool                                    fileReadComplete();
        int*                                    getRequestPipe();
        int*                                    getReponsePipe();
        std::string                             readFile ( std::string file );
        ServerBlock&                             getServerBlock();
};

std::string trimWhiteSpace(std::string& string);
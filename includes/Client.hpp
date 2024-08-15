#pragma once

#include <sstream>
#include <ctime>
#include <map>
#include <regex>
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
#include "ServerInfo.hpp"
#include <dirent.h>       // Provides functions for using DIR directory stream like opendir(), readdir()


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
        bool                                    _state = false;
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
        
        bool                                    _responseReady = false;
        ServerInfo                              _serverInfo;

        void    errorCheckRequest( void );
        bool    isValidMethod( std::string method );
        bool    isValidPath( std::string path );
        bool    isValidVersion( std::string version );

    public:
        Client();
        Client(int fd, ServerInfo& serverInfo);
        // Client(const Client& rhs);
        // Client& operator=(const Client& rhs);
        ~Client();

        void        addToBuffer( std::string bufferNew );

        void        parseBuffer ( void );
        void        printRequestMap( void );
        void        createResponse ( void );
        void        tempReponse( void);
        std::map<std::string, std::string>    getRequestMap( void );

        bool        requestComplete();

        std::time_t getTime();
        void        updateTime();

        std::string getReadBuffer();
        void        setFd(int fd);
        int         getFd();
        size_t      getWritePos();
        void        setWritePos( size_t pos );
        std::string getWriteBuffer();
        void        setFileBuffer(std::string buffer);
        void        setWriteBuffer( std::string buffer );
        void        setStatusCode( const int statusCode );
        std::string getFileBuffer();

        void        readNextChunk();
        bool        getResponseStatus();

        void        setFileFd(int fd);
        int         getFileFd();
        bool fileReadComplete();

        ServerInfo& getServerInfo();
};

std::string trimWhiteSpace(std::string& string);
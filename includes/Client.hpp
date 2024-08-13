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




enum    clientState
{
    START = 0,
    READING = 1, // Reading from file/pipe
    WRITING = 2, // Writing to file or pipe
    ERROR = 3, // Some error occured
    READY = 4 // Reading/Writing is finished and the request can be parsed
};



#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

//pending config parsing these are set

#define PORT 4040
#define MAX_CLIENTS 10
#define TIMEOUT 60
#define BUFFER_SIZE 100024

class Client
{
    private:
        int                                     _fd = -1;
        int                                     _state = START;
        int                                     _readWriteFd = -1;
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
        

        void    errorCheckRequest( void );
        void    isValidMethod( std::string method );
        void    isValidPath( std::string path );
        void    isValidVersion( std::string version );

    public:
        Client();
        Client(int fd);
        Client(const Client& rhs);
        Client& operator=(const Client& rhs); // Update!
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
        void                                    writeNextChunk();
        void                                    setReadWriteFd(int fd);
        int                                     getReadWriteFd();
        bool                                    fileReadComplete();
        int*                                    getRequestPipe();
        int*                                    getReponsePipe();
        std::string                             readFile ( std::string file );
};

std::string trimWhiteSpace(std::string& string);
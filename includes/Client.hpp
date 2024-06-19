#pragma once

#include <iostream>
#include <sstream>
#include <ctime>
#include <map>
#include <regex>
#include <vector>

enum    clientState
{
    READY = 0, // Reading is finished and the request can be parsed
    READING = 1, // Reading HTTP request
    SENDING = 2, // Sending HTTP request
    ERROR = 3 // Some error occured 
};

class Client
{
    private:
        int                                     _fd;
        int                                     _state;
        std::string                             _readBuffer;
        std::string                             _writeBuffer;
        std::string                             _fileBuffer;
        size_t                                  _writePos = 0;
        std::map<std::string, std::string>      _requestMap;
        std::map<std::string, std::string>      _responseMap;
        static const std::map<int, std::string> _ErrorMap;
        int                                     _statusCode = 0;
        std::time_t                             _time = std::time(nullptr);                     
        
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
        void                                    setFileBuffer(std::string buffer);
        void                                    setWriteBuffer( std::string buffer );
        void                                    setStatusCode( const int statusCode );
        std::map<std::string, std::string>      getRequestMap( void );
        bool                                    requestComplete();
        std::time_t                             getTime();
        void                                    updateTime();
        std::string                             createErrorResponse( void );                      
        // std::string getRequest();

};

std::string trimWhiteSpace(std::string& string);
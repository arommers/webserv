#pragma once

#include <iostream>
#include <sstream>
#include <ctime>
#include <map>
#include <regex>

class Client
{
    private:
        int                                     _fd;
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
        bool    isValidMethod( std::string method );
        bool    isValidPath( std::string path );
        bool    isValidVersion( std::string version );

    public:
        Client();
        Client(int fd);
        Client(const Client& rhs);
        Client& operator=(const Client& rhs); // Update!
        ~Client();

        void        addToBuffer( std::string bufferNew );
        std::string getReadBuffer();
        void        parseBuffer ( void );
        void        printRequestMap( void );
        void        createResponse ( void );
        void        tempReponse( void);
        void        setFd( int fd );
        int         getFd();
        size_t      getWritePos();
        void        setWritePos( size_t pos );
        std::string getWriteBuffer();
        void        setFileBuffer(std::string buffer);
        void        setWriteBuffer( std::string buffer );
        void       setStatusCode( const int statusCode );
        std::map<std::string, std::string>    getRequestMap( void );

        bool        requestComplete();
        // std::string getRequest();

        std::time_t getTime();
        void        updateTime();
};

std::string trimWhiteSpace(std::string& string);
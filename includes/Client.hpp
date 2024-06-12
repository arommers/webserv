#pragma once

#include <iostream>
#include <sstream>
#include <ctime>
#include <map>

class Client
{
    private:
        int         _fd;
        std::string _readBuffer;
        std::string _writeBuffer;
        size_t      _writePos = 0;
        std::time_t _time = std::time(nullptr);                     
        // std::map<std::string, std::string>  _headerMap;


    public:
        Client();
        Client( int fd );
        Client(const Client& rhs);
        Client& operator=(const Client rhs);
        ~Client();

        void        addToBuffer( std::string bufferNew );
        std::string getBuffer();
        // void        parseBuffer ( void );
        // void        printHeaderMap( void );
        void        setFd( int fd );
        int         getFd();
        size_t      getWritePos();
        void        setWritePos( size_t pos );
        std::string getWriteBuffer();
        void        setWriteBuffer( std::string buffer );

        bool        requestComplete();
        std::string getRequest();

        std::time_t getTime();
        void        updateTime();

};
#ifndef Client_HPP
 #define Client_HPP

#include <iostream>
#include <sstream>
#include <map>

class Client
{
private:
    int                                 _fd;
    std::string                         _writeBuffer;
    size_t                              _writePos = 0;
    std::string                         _buffer;
    std::map<std::string, std::string>  _headerMap;


public:
    Client();
    Client( int fd );
    ~Client();
    void        addToBuffer( std::string bufferNew );
    std::string getBuffer( void );
    void        parseBuffer ( void );
    void        printHeaderMap( void );
    void        setFd( int fd );
    int         getFd();
    size_t      getWritePos();
    void        setWritePos( size_t pos );
    std::string getWriteBuffer();
    void        setWriteBuffer( std::string buffer );
};

#endif
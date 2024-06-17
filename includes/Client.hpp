#ifndef Client_HPP
 #define Client_HPP

#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <regex>
#include <fstream>

enum client
{
    CGET = 0,
    CPOST = 1,
    CDELETE = 2,
    CISFILE = 3,
    CISFOLDER = 4,
};

class Client
{
private:
    int                                     _fd;
    std::string                             _writeBuffer;
    size_t                                  _writePos = 0;
    std::string                             _buffer;
    std::map<std::string, std::string>      _requestMap;
    std::map<std::string, std::string>      _responseMap;
    static const std::map<int, std::string> _ErrorMap;
    int                                     _statusCode = 0;
    
    void    errorCheckRequest( void );
    bool    isValidMethod( std::string method );
    bool    isValidPath( std::string path );
    bool    isValidVersion( std::string version );

public:
    Client();
    Client( int fd );
    ~Client();
    void        addToBuffer( std::string bufferNew );
    std::string getBuffer( void );
    void        parseBuffer ( void );
    void        executeRequest ( void );
    std::string createResponse ( void );
    void        tempReponse( void);
    void        printRequestMap( void );
    void        setFd( int fd );
    int         getFd();
    size_t      getWritePos();
    void        setWritePos( size_t pos );
    std::string getWriteBuffer();
    void        setWriteBuffer( std::string buffer );
    void        setStatusCode( const int statusCode );
    bool        checkCGI();
    int         getMethod();
    int         checkFileFolder();
    std::map<std::string, std::string>    getRequestMap( void );


};

std::string trimWhiteSpace(std::string& string);

#endif
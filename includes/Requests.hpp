#ifndef REQUESTS_HPP
 #define REQUESTS_HPP

#include <iostream>
#include <sstream>
#include <map>

class Requests
{
private:
    std::string                         _buffer;
    std::map<std::string, std::string>  _headerMap;


public:
    Requests();
    ~Requests();
    void        addToBuffer( std::string bufferNew );
    std::string getBuffer( void );
    void        parseBuffer ( void );
};

#endif
#ifndef REQUESTS_HPP
 #define REQUESTS_HPP

#include <iostream>
#include <sstream>
#include <map>

class Requests
{
private:
    std::string                         buffer;
    std::map<std::string, std::string>  headerMap;


public:
    Requests();
    ~Requests();
    void        addToBuffer( std::string bufferNew );
    std::string getBuffer( void );
    void        parseBuffer ( void );
};

#endif
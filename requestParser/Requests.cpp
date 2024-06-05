#include "../includes/Requests.hpp"

Requests::Requests()
{
    mp["GET"] = "1";
}

Requests::~Requests()
{
}

void    Requests::addToBuffer( std::string bufferNew )
{
    buffer += bufferNew;
}

std::string    Requests::getBuffer( void )
{
    return (buffer);
}

void    Requests::parseBuffer ( void )
{
    std::istringstream  stream(buffer);
    std::string         line;



    while (std::getline(stream, line))
    {
        line.erase(line.length() - 1);
        std::cout << "X" << line << "X" << std::endl;
    }

}
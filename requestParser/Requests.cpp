#include "../includes/Requests.hpp"

Requests::Requests()
{  
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
    std::string line, key, value;

    std::istringstream stream(buffer);
    if (std::getline(stream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> headerMap["Method"] >> headerMap["Path"] >> headerMap["Version"]; // Error Management missing if wrong request line format!
    }
    while (std::getline(stream, line, '\n'))
    {
        line.erase(line.length() - 1);
        std::istringstream lineStream(line);
        if (std::getline(lineStream, key, ':'))
        {
            if (std::getline(lineStream, value))
            {
                value = value.substr(1);
                headerMap[key] = value;
            }
        }
    }
    for (const auto& pair : headerMap)
    {
        std::cout << pair.first << ":::" << pair.second << "X" << std::endl;
    }

}
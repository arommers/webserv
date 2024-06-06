#include "../includes/Requests.hpp"

Requests::Requests()
{  
}

Requests::~Requests()
{
}

void    Requests::addToBuffer( std::string bufferNew )
{
    _buffer += bufferNew;
}

std::string    Requests::getBuffer( void )
{
    return (_buffer);
}

void    Requests::parseBuffer ( void )
{
    std::string line, key, value;

    // Storing the (first) request line with the method (GET/POST etc..), path and version in headerMap
    std::istringstream stream(_buffer);
    if (std::getline(stream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> _headerMap["Method"] >> _headerMap["Path"] >> _headerMap["Version"]; // Error Management missing if wrong request line format!
    }
    // Storing the rest of the incoming header in headMap
    while (std::getline(stream, line, '\n'))
    {
        line.erase(line.length() - 1);
        std::istringstream lineStream(line);
        if (std::getline(lineStream, key, ':'))
        {
            if (std::getline(lineStream, value))
            {
                value = value.substr(1);
                _headerMap[key] = value;
            }
        }
    }

    // Printing header map
    std::cout << "------- Content of header map -------\n";
    for (const auto& pair : _headerMap)
    {
        std::cout << pair.first << ":" << pair.second << std::endl;
    }

}


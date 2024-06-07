#include "../includes/Client.hpp"

Client::Client()
{ 
}

Client::~Client()
{
}

Client::Client( int fd )
{
    _fd = fd;
}

void    Client::addToBuffer( std::string bufferNew )
{
    _buffer += bufferNew;
}

std::string    Client::getBuffer( void )
{
    return (_buffer);
}

void Client::setFd ( int fd )
{
    _fd = fd;
}

int Client::getFd()
{
    return (_fd);
}

size_t      Client::getWritePos()
{
    return (_writePos);
}

void        Client::setWritePos( size_t pos )
{
    _writePos = pos;
}

std::string Client::getWriteBuffer()
{
    return (_writeBuffer);
}

void        Client::setWriteBuffer( std::string buffer )
{
    _writeBuffer = buffer;
}

void    Client::parseBuffer ( void )
{
    std::string line, key, value;

    // Storing the (first) request line with the method (GET/POST etc..), path and version in headerMap
    std::istringstream stream(_buffer);
    if (std::getline(stream, line))
    {
        std::istringstream lineStream(line);
        lineStream >> _headerMap["Method"] >> _headerMap["Path"] >> _headerMap["Version"]; // Error Management missing if wrong request line format!
    }
    // Storing the rest of the incoming header in headerMap
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
    errorCheckRequest();
}

void    Client::printHeaderMap( void )
{
    // Printing header map
    std::cout << "------- Content of header map -------\n";
    for (const auto& pair : _headerMap)
    {
        std::cout << pair.first << ":" << pair.second << std::endl;
    }   
}

std::map<std::string, std::string> Client::getHeaderMap( void )
{
    return (_headerMap);
}


void    Client::errorCheckRequest( void )
{
    if (!isValidMethod(_headerMap["Method"]) ||
        !isValidPath(_headerMap["Path"]) ||
        !isValidVersion(_headerMap["Version"]))
    {
        return ; // Return status code error and page with error!
    }
    
}

bool    Client::isValidMethod( std::string method )
{
    std::vector<std::string> validMethods = {"POST", "GET", "DELETE"};

    if (method.empty())
    {
        _statusCode = 400;
        return (false);
    }
    if (std::find(validMethods.begin(), validMethods.end(), method) != validMethods.end())
    {
        _statusCode = 405;
        return (false);
    }
    return (true);
}

bool    Client::isValidPath( std::string path )
{
    if (path.empty())
    {
        _statusCode = 400;
        return (false);
    }
    return (true);
}

bool    Client::isValidVersion( std::string version )
{
    std::regex versionRegex(R"(HTTP\/\d\.\d)");

    if (version.empty())
    {
        _statusCode = 400;
        return (false);
    }
    if (!std::regex_match(version, versionRegex))
    {
        _statusCode = 505;
        return (false);
    }
    return (true);
}

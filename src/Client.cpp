#include "../includes/Client.hpp"

const std::map<int, std::string> Client::_ErrorMap = {
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},
    {400, "Bad Formatting"},
    {401, "Unauthorized"},
    {404, "Not Found"},
    {500, "Internal Server Error"},
    {503, "Service Unavailable"},
};

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
        lineStream >> _requestMap["Method"] >> _requestMap["Path"] >> _requestMap["Version"]; // Error Management missing if wrong request line format!
    }
    // Storing the rest of the incoming header in headerMap
    while (std::getline(stream, line, '\n'))
    {
        std::istringstream lineStream(line);
        if (std::getline(lineStream, key, ':'))
        {
            if (std::getline(lineStream, value))
            {
                value = trimWhiteSpace(value);
                _requestMap[key] = value;
            }
            else if (!key.empty())
                _requestMap["Body"] = key;
        }
    }
    errorCheckRequest();
}

void    Client::printHeaderMap( void )
{
    // Printing header map
    std::cout << "------- Content of header map -------\n";
    for (const auto& pair : _requestMap)
    {
        std::cout << pair.first << ":" << pair.second << std::endl;
    }
}

std::map<std::string, std::string> Client::getHeaderMap( void )
{
    return (_requestMap);
}


void    Client::errorCheckRequest( void )
{
    if (!isValidMethod(_requestMap["Method"]) ||
        !isValidPath(_requestMap["Path"]) ||
        !isValidVersion(_requestMap["Version"]))
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


std::string trimWhiteSpace(std::string& string)
{
    size_t start = string.find_first_not_of(" \n\t\r");
    size_t end = string.find_last_not_of(" \n\t\r");

    return string.substr(start, end - start + 1);
}

void Client::tempReponse( void)
{
    _statusCode = 400;
    std::string htmlContent = "<html>"
                            "<head><title>" + std::to_string(_statusCode) + " " + _ErrorMap.at(_statusCode) + "</title></head>"
                            "<body>"
                            "<h1>" + std::to_string(_statusCode) + " " + _ErrorMap.at(_statusCode) + "</h1>"
                            "<p>The request could not be understood by the server due to malformed syntax.</p>"
                            "</body>"
                            "</html>\r\t\r\t";
    if (_statusCode != 200){
        _responseMap["Body"] = htmlContent;
        _responseMap["Content-Type"] = "text/html";
    }
    else{
        _responseMap["Body"] = "Hello Big World!";
        _responseMap["Content-Type"] = "text/plain";

    }

    _responseMap["Content-Length"] = std::to_string(_responseMap["Body"].size());

}

std::string Client::createResponse ( void )
{
    std::string responseMessage;

    responseMessage = _requestMap.at("Version") + " " + std::to_string(_statusCode) + " " + _ErrorMap.at(_statusCode) + "\n";
    responseMessage += "Content-Type: " + _responseMap.at("Content-Type") + "\n";
    if (_responseMap.count("Body")){
        responseMessage += "Content-Length: " + _responseMap.at("Content-Length") + "\r\n\r\n";
        responseMessage += _responseMap.at("Body");
    }

    


    return responseMessage;
}
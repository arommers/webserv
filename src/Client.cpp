#include "../includes/Client.hpp"

const std::map<int, std::string> Client::_ErrorMap = {
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},
    {400, "Bad Formatting"},
    {401, "Unauthorized"},
    {404, "Not Found"},
    {405, "Method not allowed"},
    {500, "Internal Server Error"},
    {503, "Service Unavailable"},
};

Client::Client() {}

Client::~Client() { }

Client::Client( int fd ): _fd(fd) {}

Client::Client(const Client& rhs)
{
    _fd = rhs._fd;
    _readBuffer = rhs._readBuffer;
    _writeBuffer = rhs._writeBuffer;
    _writePos = rhs._writePos;
    _time = rhs._time;
}

Client& Client::operator=(const Client& rhs)
{
    if (this != &rhs)
    {
        _fd = rhs._fd;
        _readBuffer = rhs._readBuffer;
        _writeBuffer = rhs._writeBuffer;
        _writePos = rhs._writePos;
        _time = rhs._time;
    }
    return *this;
}

void    Client::addToBuffer( std::string bufferNew )
{
    _readBuffer += bufferNew;
}

bool    Client::requestComplete()
{
    size_t pos = _readBuffer.find("\r\n\r\n");

    if (pos == std::string::npos)
        return false;
    
    std::string headers = _readBuffer.substr(0, pos + 4);
    size_t posContent = headers.find("Content-Length:");

    if (posContent == std::string::npos)
        return true;
    
    size_t contentEnd = headers.find("\r\n", posContent);
    std::string content = headers.substr(posContent + 15, contentEnd - posContent - 15);
    int contentLength = std::stoi(content);

    size_t bodyBegin = pos + 4;
    size_t bodyLength = bodyBegin + contentLength;

    return _readBuffer.size() >= bodyLength;
}

void    Client::updateTime()
{
    _time = std::time(nullptr);
}

std::time_t Client::getTime()
{
    return _time;
}

void    Client::parseBuffer ( void )
{
    std::string line, key, value;

    // Storing the (first) request line with the method (GET/POST etc..), path and version in headerMap
    std::istringstream stream(_readBuffer);
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
    //errorCheckRequest();
}

void    Client::printRequestMap( void )
{
    // Printing header map
    std::cout << "------- Content of header map -------\n";
    for (const auto& pair : _requestMap)
    {
        std::cout << pair.first << ":" << pair.second << std::endl;
    }   
}

std::map<std::string, std::string> Client::getRequestMap( void )
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
    else
        _responseMap["Content-Length"] = std::to_string(_responseMap["Body"].size());
    // else{
    //     _responseMap["Body"] = "Hello Big World!";
    //     _responseMap["Content-Type"] = "text/plain";

    // }
}

void Client::createResponse ( void )
{
    std::string responseMessage;

    if (_statusCode == 0)
        setStatusCode(200);
    // if (getState() == ERROR)
    // {
    //     _writeBuffer = createErrorResponse();
    //     setState(READY);
    // }
    // else {
    _responseMap["Content-Type"] = "text/html";
    responseMessage = _requestMap.at("Version") + " " + std::to_string(_statusCode) + " " + _ErrorMap.at(_statusCode) + "\r\n";
    responseMessage += "Content-Type: " + _responseMap.at("Content-Type") + "\r\n";
    if (!_fileBuffer.empty())
    {
        responseMessage += "Content-Length: " + std::to_string(_fileBuffer.size()) + "\r\n\r\n";
        responseMessage += _fileBuffer;
    }
    else
        responseMessage += "\r\n";
    _writeBuffer = responseMessage;
    // }

}

void Client::readNextChunk()
{
    char buffer[BUFFER_SIZE];
    int bytesRead = read(_fileFd, buffer, BUFFER_SIZE);

    if (bytesRead < 0)
    {
        std::cerr << "Failed to read file: " << strerror(errno) << std::endl;
        setStatusCode(404);
        close(_fileFd);
        _fd = -1;
        _responseReady = true;
        return;
    }
    else if (bytesRead == 0)
    {
        close(_fileFd);
        _fileFd = -1;
        _responseReady = true;
        createResponse();
        return;
    }

    _fileBuffer.append(buffer, bytesRead);

}


// void    Client::readNextChunk()
// {
//     char    buffer[BUFFER_SIZE];
//     int     bytesRead = read(_fd, buffer, BUFFER_SIZE);
    
//     if (bytesRead < 0)
//     {
//         std::cerr << "Failed to read file: " << strerror(errno) << std::endl;
//         setStatusCode(404);
//         close(_fd);
//         _responseReady = true;
//         return;
//     }
//     else if (bytesRead == 0)
//     {
//         close(_fd);
//         _responseReady = true;
//         createResponse();
//         return;
//     }
//     _fileBuffer.append(buffer, bytesRead);
// }

// Getters and Setters


bool Client::getResponseStatus()
{
    return _responseReady;
}

std::string Client::getFileBuffer()
{
    return _fileBuffer;
}

std::string    Client::getReadBuffer( void )
{
    return (_readBuffer);
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

void        Client::setFileBuffer(std::string buffer)
{
    _fileBuffer = buffer;
}

void Client::setStatusCode( const int statusCode )
{
    _statusCode = statusCode;
}

void Client::setFileFd(int fd)
{
    _fileFd = fd;
}

int Client::getFileFd()
{
    return _fileFd;
}

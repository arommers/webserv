#include "../includes/Client.hpp"

const std::map<int, std::string> Client::_ReasonPhraseMap = {
    {200, "OK"},
    {201, "Created"},
    {204, "No Content"},
	{301, "Moved Permanently"},
	{302, "Found"},
    {400, "Bad Formatting"},
    {401, "Unauthorized"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {500, "Internal Server Error"},
    {503, "Service Unavailable"},
};

// --- Constructors/ Deconstructor ---
Client::Client() {}
Client::~Client() {}
Client::Client(int fd, ServerBlock& ServerBlock): _fd(fd), _ServerBlock(ServerBlock) {}

// --- Client Functions ---

void	Client::resetClientData( void )
{
	_readBuffer.clear();
	_writeBuffer.clear();
	_fileBuffer.clear();
	_writePos = 0;
	_requestMap.clear();
	_responseMap.clear();
	_statusCode = 0;
	_fd = -1;
	_state = PARSE;
	_readWriteFd = -1;
}

bool	Client::requestComplete()
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

void	Client::parseBuffer ( void )
{
	std::string line, key, value;
	bool startBody = false;

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
		if (startBody == false && line == "\r"){
			startBody = true;
		}
		if (startBody == true){
			_requestMap["Body"] += line + '\n';
		}
		else if (std::getline(lineStream, key, ':'))
		{
			if (std::getline(lineStream, value))
			{            
				value = trimWhiteSpace(value);
				_requestMap[key] = value;
			}
		}
	}
	isValidMethod(_requestMap["Method"]);
	isValidPath(_requestMap["Path"]);
	isValidVersion(_requestMap["Version"]); 
}

void	Client::createResponse()
{
	std::string responseMessage;

	if (_statusCode == 0)
		setStatusCode(200);

	// Handle redirect responses
	if (_statusCode == 301 || _statusCode == 302)
	{
		responseMessage = _requestMap.at("Version") + " " + std::to_string(_statusCode) + " " + _ReasonPhraseMap.at(_statusCode) + "\r\n";
		responseMessage += "Location: " + _responseMap["Location"] + "\r\n";
		responseMessage += "Content-Length: 0\r\n\r\n";  // Usually no body for redirects
	}
	else
	{
		// Handle regular responses
		_responseMap["Content-Type"] = "text/html";
		responseMessage = _requestMap.at("Version") + " " + std::to_string(_statusCode) + " " + _ReasonPhraseMap.at(_statusCode) + "\r\n";
		responseMessage += "Content-Type: " + _responseMap["Content-Type"] + "\r\n";
		if (!_fileBuffer.empty())
		{
			responseMessage += "Content-Length: " + std::to_string(_fileBuffer.size()) + "\r\n\r\n";
			responseMessage += _fileBuffer;
		}
		else
			responseMessage += "\r\n";
	}
	_writeBuffer = responseMessage;
}

void	Client::readNextChunk()
{
	char buffer[BUFFER_SIZE];
	int bytesRead = read(_readWriteFd, buffer, BUFFER_SIZE);
	if (bytesRead < 0)
	{
		std::cerr << "Failed to read file: " << strerror(errno) << std::endl;
		close(_readWriteFd);
		setStatusCode(500);
		_fd = -1;
	}
	else if (bytesRead == 0)
	{
		close(_readWriteFd);
		setState(READY);
	}
	else
		_fileBuffer.append(buffer, bytesRead);
}

void	Client::writeNextChunk()
{
	std::string buffer;
	int bytesWritten;

	buffer = getWriteBuffer().substr(0,BUFFER_SIZE);
	bytesWritten = write(getReadWriteFd(), buffer.c_str(), buffer.length());
	if (bytesWritten < 0)
	{
		std::cerr << "Failed to write to fd: " << strerror(errno) << std::endl;
		close(_readWriteFd);
		setState(500);
		_fd = -1;
		return ;
	}
	_writeBuffer.erase(0, bytesWritten);
	if (getWriteBuffer().empty())
		setState(READY);
}

// --- Utils ---
bool	Client::detectError()
{
	if (std::find(_statusCheck.begin(), _statusCheck.end(), _statusCode) != _statusCheck.end())
		return (true);
	return (false);
}

void	Client::updateTime()
{
	_time = std::time(nullptr);
}

void	Client::addToBuffer( std::string bufferNew )
{
	_readBuffer += bufferNew;
}

void	Client::printRequestMap( void )
{
	// Printing header map
	std::cout << "------- Content of header map -------\n";
	for (const auto& pair : _requestMap)
	{
		std::cout << pair.first << ":" << pair.second << std::endl;
	}
	std::cout << "\n------- Content of header map -------\n";
}

void	Client::isValidMethod( std::string method )
{
	std::vector<std::string> validMethods = {"POST", "GET", "DELETE"};

	if (method.empty()){
		setStatusCode(400);
	}
	else if (std::find(validMethods.begin(), validMethods.end(), method) == validMethods.end()){
		setStatusCode(405);
	}
}

void	Client::isValidPath( std::string path )
{
	if (path.empty()){
		setStatusCode(400);
	}
}

void	Client::isValidVersion( std::string version )
{
	std::regex versionRegex(R"(HTTP\/\d\.\d)");

	if (version.empty()){
		setStatusCode(400);
	}
	else if (!std::regex_match(version, versionRegex)){
		setStatusCode(505);
	}
}

std::string	trimWhiteSpace(std::string& string)
{
	size_t start = string.find_first_not_of(" \n\t\r");
	size_t end = string.find_last_not_of(" \n\t\r");

	return string.substr(start, end - start + 1);
}

// --- Getter ---
int	Client::getStatusCode()
{
	return (_statusCode);
}

int	Client::getReadWriteFd()
{
	return _readWriteFd;
}

int*	Client::getRequestPipe()
{
	return (_requestPipe);
}

int*	Client::getResponsePipe()
{
	return (_responsePipe);
}

std::time_t	Client::getTime()
{
	return _time;
}

std::map<std::string, std::string>	Client::getRequestMap( void )
{
	return (_requestMap);
}

ServerBlock&	Client::getServerBlock()
{
	return _ServerBlock;
}

std::string	Client::getReadBuffer( void )
{
	return (_readBuffer);
}

int	Client::getFd()
{
	return (_fd);
}

size_t	Client::getWritePos()
{
	return (_writePos);
}

std::string	Client::getWriteBuffer()
{
	return (_writeBuffer);
}

int	Client::getState()
{
	return (_state);
}

std::map<std::string, std::string>&	Client::getResponseMap()
{
	return (_responseMap);
}

// --- Setter --- 
void	Client::setReadWriteFd(int fd)
{
	_readWriteFd = fd;
}

void	Client::setFd ( int fd )
{
	_fd = fd;
}

void	Client::setWritePos( size_t pos )
{
	_writePos = pos;
}

void	Client::setWriteBuffer( std::string buffer )
{
	_writeBuffer = buffer;
}

void	Client::setFileBuffer(std::string buffer)
{
	_fileBuffer = buffer;
}

void	Client::setState (const int state)
{
	_state = state;
}

void	Client::setStatusCode( const int statusCode )
{
	_statusCode = statusCode;
	if (detectError()){
		setState(ERROR);
	}
}

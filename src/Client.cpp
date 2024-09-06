#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"

Client::Client(int fd, ServerBlock& ServerBlock) : _ServerBlock(ServerBlock)
{
	_fd = fd;
}

Client::~Client() {}

// Utils

void	Client::resetClientData( void )
{
	getReadBuffer().clear();
	getWriteBuffer().clear();
	getFileBuffer().clear();
	setWritePos(0);
	getRequestMap().clear();
	getResponseMap().clear();
	_statusCode = 0;
	_state = PARSE;
	_readWriteFd = -1;
}

bool	Client::requestComplete()
{
	size_t pos = getReadBuffer().find("\r\n\r\n");

	if (pos == std::string::npos)
		return false;
	
	std::string headers = getReadBuffer().substr(0, pos + 4);
	size_t posContent = headers.find("Content-Length:");

	if (posContent == std::string::npos)
		return true;
	
	size_t contentEnd = headers.find("\r\n", posContent);
	std::string content = headers.substr(posContent + 15, contentEnd - posContent - 15);
	int contentLength = std::stoi(content);

	size_t bodyBegin = pos + 4;
	size_t bodyLength = bodyBegin + contentLength;

	return getReadBuffer().size() >= bodyLength;
}

void	Client::parseBuffer ( void )
{
	std::string line, key, value;
	bool startBody = false;

	// Storing the (first) request line with the method (GET/POST etc..), path and version in headerMap
	std::istringstream stream(getReadBuffer());
	if (std::getline(stream, line))
	{
		std::istringstream lineStream(line);
		lineStream >> getRequestMap()["Method"] >> getRequestMap()["Path"] >> getRequestMap()["Version"]; // Error Management missing if wrong request line format!
	}
	// Storing the rest of the incoming header in headerMap
	while (std::getline(stream, line, '\n'))
	{
		std::istringstream lineStream(line);
		if (startBody == false && line == "\r")
			startBody = true;
		if (startBody == true)
			getRequestMap()["Body"] += line + '\n';
		else if (std::getline(lineStream, key, ':'))
		{
			if (std::getline(lineStream, value))
			{            
				value = trimWhiteSpace(value);
				getRequestMap()[key] = value;
			}
		}
	}
	isValidMethod(getRequestMap()["Method"]);
	isValidPath(getRequestMap()["Path"]);
	isValidVersion(getRequestMap()["Version"]); 
}

void	Client::createResponse()
{
	std::string responseMessage;

	if (_statusCode == 0)
		setStatusCode(200);
	// Handle redirect responses
	if (_statusCode == 301 || _statusCode == 302)
	{
		responseMessage = getRequestMap().at("Version") + " " + std::to_string(_statusCode) + " " + getStatusMessage(_statusCode) + "\r\n";
		responseMessage += "Location: " + getResponseMap()["Location"] + "\r\n";
		responseMessage += "Content-Length: 0\r\n\r\n";  // Usually no body for redirects
	}
	else
	{
		// Handle regular responses
		getResponseMap()["Content-Type"] = "text/html";
		responseMessage = getRequestMap().at("Version") + " " + std::to_string(_statusCode) + " " + getStatusMessage(_statusCode) + "\r\n";
		responseMessage += "Content-Type: " + getResponseMap()["Content-Type"] + "\r\n";
		if (!getFileBuffer().empty())
		{
			responseMessage += "Content-Length: " + std::to_string(getFileBuffer().size()) + "\r\n\r\n";
			responseMessage += getFileBuffer();
		}
		else if (getRequestMap().at("Method") == "GET")
		{
			responseMessage += "Content-Length: 0\r\n\r\n";
		}
		else
			responseMessage += "\r\n";
	}
	setWriteBuffer(responseMessage);
	setState(SENDING);
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
		getFileBuffer().append(buffer, bytesRead);
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
	if (bytesWritten == 0)
	{
		std::cerr << "Warning: write() returned 0, no data was written. Possibly a closed connection." << std::endl;
		close(_readWriteFd);
		setState(500);
		_fd = -1;
		return;
	}
	getWriteBuffer().erase(0, bytesWritten);
	if (getWriteBuffer().empty())
		setState(READY);
}


void	Client::addToBuffer( std::string bufferNew )
{
	getReadBuffer() += bufferNew;
}

std::string	trimWhiteSpace(std::string& string)
{
	size_t start = string.find_first_not_of(" \n\t\r");
	size_t end = string.find_last_not_of(" \n\t\r");

	return string.substr(start, end - start + 1);
}

// --- Getters ---
int	Client::getStatusCode()
{
	return (_statusCode);
}

int	Client::getReadWriteFd()
{
	return _readWriteFd;
}

ServerBlock&	Client::getServerBlock()
{
	return _ServerBlock;
}

int	Client::getFd()
{
	return (_fd);
}

int	Client::getState()
{
	return (_state);
}

// --- Setters --- 
void	Client::setReadWriteFd(int fd)
{
	_readWriteFd = fd;
}

void	Client::setFd ( int fd )
{
	_fd = fd;
}

void	Client::setState (const int state)
{
	_state = state;
}

void	Client::setStatusCode( const int statusCode )
{
	_statusCode = statusCode;
	if (detectError(statusCode))
		setState(ERROR);
}

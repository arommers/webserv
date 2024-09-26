#include "../includes/Client.hpp"

Client::Client(int fd, ServerBlock& ServerBlock) : _ServerBlock(ServerBlock)
{
	_fd = fd;
}

Client::~Client() {}

// Utils

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

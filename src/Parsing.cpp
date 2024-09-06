#include "../includes/Parsing.hpp"

Parsing::Parsing(){};

Parsing::~Parsing(){};

std::unordered_map<std::string, std::string>&	Parsing::getResponseMap()
{
	return (_responseMap);
}

std::unordered_map<std::string, std::string>&	Parsing::getRequestMap( void )
{
	return (_requestMap);
}

void	Parsing::printRequestMap( void )
{
    std::unordered_map<std::string, std::string> requestMap = getRequestMap();
    
	std::cout << "------- Content of header map -------\n";
	for (const auto& pair : requestMap)
	{
		std::cout << pair.first << ":" << pair.second << std::endl;
	}
	std::cout << "\n------- Content of header map -------\n";
}


std::string&	Parsing::getReadBuffer( void )
{
	return (_readBuffer);
}

std::string&	Parsing::getWriteBuffer()
{
	return (_writeBuffer);
}

void	Parsing::setWriteBuffer( std::string buffer )
{
	_writeBuffer = buffer;
}

void	Parsing::setFileBuffer(std::string buffer)
{
	_fileBuffer = buffer;
}

std::string& Parsing::getFileBuffer()
{
    return (_fileBuffer);
}

size_t	Parsing::getWritePos()
{
	return (_writePos);
}

void	Parsing::setWritePos( size_t pos )
{
	_writePos = pos;
}

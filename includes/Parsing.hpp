#pragma once


#include <ctime>
#include <unordered_map>
#include <iostream>       // Input and output through streams
#include <cstring>        // Manipulate C-style strings and arrays


class Parsing{
    private:
    	size_t									            _writePos = 0;
		std::unordered_map<std::string, std::string>		_requestMap;
		std::unordered_map<std::string, std::string>		_responseMap;
        std::string								            _readBuffer;
		std::string								            _writeBuffer;
		std::string								            _fileBuffer;

    public:
        Parsing();
        ~Parsing();
        
        std::unordered_map<std::string, std::string>&	getRequestMap( void );
        std::unordered_map<std::string, std::string>&	getResponseMap( void );
        void								            printRequestMap( void );
        void								            setFileBuffer(std::string buffer);
		void								            setWriteBuffer( std::string buffer );
        std::string&						            getReadBuffer();
		std::string&						            getWriteBuffer();
        std::string&                                    getFileBuffer();
        size_t								            getWritePos();
		void								            setWritePos( size_t pos );



};
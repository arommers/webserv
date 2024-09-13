#pragma once


#include <ctime>
#include <unordered_map>
#include <iostream>       // Input and output through streams
#include <cstring>        // Manipulate C-style strings and arrays
#include <regex>
#include <vector>

class Client;

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

        //Utils
		void									        parseBuffer ( void );
        void									        detectParsingError( Client& client );
        void	                                        addToBuffer( std::string bufferNew );
        bool	                                        requestComplete( Client& client );
		void									        createResponse (Client& client);
        void	                                        buildResponse(Client& client);
        void                                            buildRedirectReponse(Client& client);
        void								            printRequestMap( void );
        void									        isValidMethod( std::string method, Client& client );
		void									        isValidPath( std::string path, Client& client );
		void									        isValidVersion( std::string version, Client& client );
        
        // Getters
        std::unordered_map<std::string, std::string>&	getRequestMap( void );
        std::unordered_map<std::string, std::string>&	getResponseMap( void );
        std::string&						            getReadBuffer();
		std::string&						            getWriteBuffer();
        std::string&                                    getFileBuffer();
        size_t								            getWritePos();

        // Setters
        void								            setFileBuffer(std::string buffer);
		void								            setWriteBuffer( std::string buffer );
		void								            setWritePos( size_t pos );



};
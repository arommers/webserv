#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>





/*
	default_error_pages :
	- Setup default error pages.
		- 400 \ 403 \ 404 \ 405 \ 406 \ 409 \ 410 \ 500 \ 505

		html files (.html)

		400 Bad Request
		403 Forbidden
		404 Not Found
		405 Method Not Allowed
		406 Not Acceptable
		409 Conflict
		410 Gone
		500 Internal Server Error
		505 HTTP Version Not Supported



for testig run:
----------------
g++ -o simple_test main_test.cpp
./simple_test
http://localhost:8080/

*/




// JUST FOR TESTING
const std::string BAD_REQUEST_PAGE = "400.html";
const std::string FORBIDDEN_PAGE = "403.html";
const std::string NOT_FOUND_PAGE = "404.html";
const std::string NOT_ALLOWED_PAGE = "405.html";
const std::string NOT_ACCEPT_PAGE = "406.html";
const std::string CONFLICT_PAGE = "409.html";
const std::string GONE_PAGE = "410.html";
const std::string INTERN_SERVER_ERROR_PAGE = "500.html";
const std::string VERSION_NOT_SUPPORTED_PAGE = "505.html";

const int BUFFER_SIZE = 4096;

std::string readFile(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file.is_open()) 
	{
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void handleClient(int clientSocket, const std::string _page) 
{
    char buffer[BUFFER_SIZE];
    read(clientSocket, buffer, sizeof(buffer) - 1);

    std::string errorContent = readFile(_page);
    std::string response = "HTTP/1.1 error\r\nContent-Type: text/html\r\n\r\n" + errorContent;
    send(clientSocket, response.c_str(), response.size(), 0);

    close(clientSocket);
}

// Simple main to test/see my html.
int main() 
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) 
	{
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) 
	{
        std::cerr << "Failed to bind socket." << std::endl;
        return 1;
    }

    if (listen(serverSocket, 10) == -1) 
	{
        std::cerr << "Failed to listen on socket." << std::endl;
        return 1;
    }

    std::cout << "Server is running on port 8080..." << std::endl;

    while (true) 
	{
        sockaddr_in clientAddr;
        socklen_t clientAddrSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == -1) 
		{
            std::cerr << "Failed to accept connection." << std::endl;
            continue;
        }

		// // calls the different html error files.
		// // testing them
        // handleClient(clientSocket, BAD_REQUEST_PAGE);
		// handleClient(clientSocket, FORBIDDEN_PAGE);
		// handleClient(clientSocket, NOT_FOUND_PAGE);
		// handleClient(clientSocket, NOT_ALLOWED_PAGE);
		// handleClient(clientSocket, NOT_ACCEPT_PAGE);
		// handleClient(clientSocket, CONFLICT_PAGE);
		// handleClient(clientSocket, GONE_PAGE);
		handleClient(clientSocket, INTERN_SERVER_ERROR_PAGE);
		// handleClient(clientSocket, VERSION_NOT_SUPPORTED_PAGE);
		
    }

    close(serverSocket);
    return 0;
}
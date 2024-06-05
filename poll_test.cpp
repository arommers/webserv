#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <unordered_map>

#include <signal.h>
#include "includes/Requests.hpp"

#define PORT 4040
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

struct Client {
    int fd;
    std::string writeBuffer;
    size_t writePos = 0;
};

void    sig_handler(sig_atomic_t s)
{
    std::cout << "\nExit signal caught\n";
    exit(0);
}

int main() {
    
    int opt = 1;
    Requests newRequests;
    struct sockaddr_in address;
    int addrLen = sizeof(address);
    int serverSocket, newSocket;
    const char* message = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\nContent-Type: text/plain\r\n\r\nHello World!";
    std::vector<struct pollfd> fds;
    std::unordered_map<int, Client> clients;

    signal (SIGINT, sig_handler);
    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Allow port to be reused after exiting the program (this is temporary!)
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));


    // Configuring the address struct of the socket
    address.sin_family = AF_INET; // address family
    address.sin_addr.s_addr = INADDR_ANY; // accepts connections from any IP on the host
    address.sin_port = htons(PORT); // ensures the port number is correctly formatted

    // Binding the socket to the address and the port
    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&address), addrLen) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Socket starts listening mode
    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Add the server socket to the poll file descriptor set
    struct pollfd server_fd;
    server_fd.fd = serverSocket;
    server_fd.events = POLLIN; // We are interested in read events (incoming connections)
    fds.push_back(server_fd);

    while (true) {
        // Use poll to wait for events
        int poll_count = poll(fds.data(), fds.size(), -1);
        if (poll_count == -1) {
            std::cerr << "Poll failed: " << strerror(errno) << std::endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == serverSocket) {
                    // Accept new connection
                    if ((newSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&address), reinterpret_cast<socklen_t*>(&addrLen))) < 0) {
                        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
                    } else {
                        std::cout << "New connection, socket fd is " << newSocket << ", ip is : " << inet_ntoa(address.sin_addr) << ", port : " << ntohs(address.sin_port) << std::endl;

                        // Add new client socket to the poll file descriptor set
                        struct pollfd client_fd;
                        client_fd.fd = newSocket;
                        client_fd.events = POLLIN; // We are interested in read events (incoming data)
                        fds.push_back(client_fd);

                        // Initialize client write buffer
                        clients[newSocket] = Client{newSocket};
                    }
                } else {
                    // Handle data from a client socket
                    char buffer[BUFFER_SIZE] = {0};
                    int valread = read(fds[i].fd, buffer, BUFFER_SIZE);
                    if (valread == 0) {
                        // Client disconnected
                        std::cout << "Client disconnected, socket fd is " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        clients.erase(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        --i; // Adjust index after removal
                    } else {

                        newRequests.addToBuffer(buffer);
                        // Prepare response to be sent
                        std::cout << "Received message: " << buffer << std::endl;
                        clients[fds[i].fd].writeBuffer = message;
                        clients[fds[i].fd].writePos = 0;
                        fds[i].events = POLLIN | POLLOUT; // Monitor for both read and write events
                    }
                }
            }
           

            else if (fds[i].revents & POLLOUT) {
                // std::cout << "\nFinal request: \n" << "###" << newRequests.getBuffer() << "###" << std::endl;
                newRequests.parseBuffer();
                Client &client = clients[fds[i].fd];
                if (client.writePos < client.writeBuffer.size()) {
                    int bytesSent = send(client.fd, client.writeBuffer.c_str() + client.writePos, client.writeBuffer.size() - client.writePos, 0);
                    if (bytesSent < 0) {
                        std::cerr << "Send failed: " << strerror(errno) << std::endl;
                        close(client.fd);
                        clients.erase(client.fd);
                        fds.erase(fds.begin() + i);
                        --i; // Adjust index after removal
                    } else {
                        client.writePos += bytesSent;
                    }
                }

                // If all data has been sent, stop monitoring for writability
                if (client.writePos >= client.writeBuffer.size()) {
                    fds[i].events = POLLIN;
                }

            }
        }
    }

    close(serverSocket);
    return 0;
}

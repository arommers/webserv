#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>

bool isPipeEmpty(int fd) {
    int bytesAvailable = 0;

    if (ioctl(fd, FIONREAD, &bytesAvailable) == -1) {
        // Handle error
        std::cerr << "ioctl failed: " << strerror(errno) << std::endl;
        return true;  // or handle error accordingly
    }

    return bytesAvailable == 0;
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "pipe failed: " << strerror(errno) << std::endl;
        return 1;
    }

    // Write some data to the pipe
    const char* message = "Hello, world!";
    write(pipefd[1], message, strlen(message));

    // Check if the pipe is empty
    if (isPipeEmpty(pipefd[0])) {
        std::cout << "Pipe is empty." << std::endl;
    } else {
        std::cout << "Pipe has data." << std::endl;
    }

    // Cleanup
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
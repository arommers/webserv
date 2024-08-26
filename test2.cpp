#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

bool isPipeEmpty(int fd) {
    int bytesAvailable = 0;
    if (ioctl(fd, FIONREAD, &bytesAvailable) == -1) {
        perror("ioctl");
        return true;  // Assuming the pipe is empty on error
    }
    return bytesAvailable == 0;
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child process
        close(pipefd[0]);  // Close read end

        // Redirect STDERR to the write end of the pipe
        if (dup2(pipefd[1], STDERR_FILENO) == -1) {
            perror("dup2");
            return 1;
        }

        // Now exec the command (example: an invalid command to generate an error)
        execl("time.cgi", "time.cgi", (char *)NULL);

        // If exec fails, exit the child process
        perror("execl");
        exit(1);
    } else {
        // Parent process
        close(pipefd[1]);  // Close write end

        // Wait for the child process to finish
        waitpid(pid, NULL, 0);

        // Check if the STDERR pipe is empty
        if (isPipeEmpty(pipefd[0])) {
            std::cout << "Pipe is empty, no error output." << std::endl;
        } else {
            std::cout << "Pipe has data, there is error output." << std::endl;

            // Optionally, read and display the data
            char buffer[1024];
            ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';  // Null-terminate the string
                std::cout << "Error output: " << buffer << std::endl;
            }
        }

        // Close the read end
        close(pipefd[0]);
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <grep_argument>\n", argv[0]);
        return 1;
    }

    int fd1[2];  // Pipe from P1 to P2
    int fd2[2];  // Pipe from P2 to P3

    // Create pipes
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    pid_t p1 = fork();  // Fork the first child (P2)
    if (p1 < 0) {
        perror("Fork failed");
        return 1;
    }

    if (p1 == 0) {
        // Child process (P2) - Executes `grep`
        close(fd1[1]);  // Close write end of fd1
        dup2(fd1[0], STDIN_FILENO);  // Redirect stdin to read from fd1
        close(fd1[0]);

        close(fd2[0]);  // Close read end of fd2
        dup2(fd2[1], STDOUT_FILENO);  // Redirect stdout to write to fd2
        close(fd2[1]);

        execlp("grep", "grep", argv[1], NULL);  // Execute `grep <argument>`
        perror("execlp failed");
        exit(1);
    } else {
        pid_t p2 = fork();  // Fork the second child (P3)
        if (p2 < 0) {
            perror("Fork failed");
            return 1;
        }

        if (p2 == 0) {
            // Child's child process (P3) - Executes `sort`
            close(fd1[0]);  // Close unused ends of fd1
            close(fd1[1]);
            close(fd2[1]);  // Close write end of fd2
            dup2(fd2[0], STDIN_FILENO);  // Redirect stdin to read from fd2
            close(fd2[0]);

            execlp("sort", "sort", NULL);  // Execute `sort`
            perror("execlp failed");
            exit(1);
        } else {
            // Parent process (P1) - Executes `cat scores`
            close(fd1[0]);  // Close read end of fd1
            dup2(fd1[1], STDOUT_FILENO);  // Redirect stdout to write to fd1
            close(fd1[1]);

            close(fd2[0]);  // Close unused ends of fd2
            close(fd2[1]);

            execlp("cat", "cat", "scores", NULL);  // Execute `cat scores`
            perror("execlp failed");
            exit(1);
        }
    }

    return 0;
}

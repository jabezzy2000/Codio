#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
    int fd1[2];  // Pipe from P1 to P2
    int fd2[2];  // Pipe from P2 to P1
    pid_t p;
  
    // Pipes creation
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
  
    p = fork();
  
    if (p < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (p > 0) {  // Parent process (P1)
        char input_str1[100];
        char input_str2[100];

        // Get input string from user
        printf("Enter a string to concatenate: ");
        scanf("%s", input_str1);

        close(fd1[0]);  // Close reading end of first pipe
        close(fd2[1]);  // Close writing end of second pipe

        // Write input string to P2
        write(fd1[1], input_str1, strlen(input_str1) + 1);
        close(fd1[1]);  // Close writing end of first pipe after sending

        // Wait for child to complete concatenation and send string back
        wait(NULL);
        read(fd2[0], input_str2, 100);
        close(fd2[0]);  // Close reading end of second pipe

        // Concatenate the received string with "gobison.org"
        strcat(input_str2, "gobison.org");

        printf("Final concatenated string: %s\n", input_str2);
    } else {  // Child process (P2)
        execl("./pipes_process2", "pipes_process2", NULL);  // Execute the second program
    }

    return 0;
}

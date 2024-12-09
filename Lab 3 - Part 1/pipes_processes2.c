#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

int main()
{
    int fd1[2];  // Pipe from P1 to P2
    int fd2[2];  // Pipe from P2 to P1

    char fixed_str1[] = "howard.edu";
    char input_str1[100];
    char input_str2[100];

    // Pipes should already be created by parent, close ends not needed here
    close(fd1[1]);  // Close writing end of first pipe
    close(fd2[0]);  // Close reading end of second pipe

    // Read string from P1
    read(fd1[0], input_str1, 100);
    close(fd1[0]);  // Close reading end of first pipe

    // Concatenate with "howard.edu"
    strcat(input_str1, fixed_str1);
    printf("Concatenated string: %s\n", input_str1);

    // Get additional input from user
    printf("Enter another string to concatenate: ");
    scanf("%s", input_str2);

    // Send the second string to P1 via the second pipe
    write(fd2[1], input_str1, strlen(input_str1) + 1);
    close(fd2[1]);  // Close writing end of second pipe

    return 0;
}

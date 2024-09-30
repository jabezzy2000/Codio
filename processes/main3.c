#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    pid_t pid1, pid2;
    int status;

    srandom(time(NULL));
    pid1 = fork();

    if (pid1 == 0) {
        int iterations = random() % 30 + 1; // Random number of iterations, 1 to 30
        for (int i = 0; i < iterations; i++) {
            int sleepTime = random() % 10 + 1; // Random sleep time, 1 to 10 seconds
            printf("Child Pid: %d is going to sleep for %d seconds!\n", getpid(), sleepTime);
            sleep(sleepTime);
            printf("Child Pid: %d is awake! Where is my Parent: %d?\n", getpid(), getppid());
        }
        exit(0); 
    } else {
        
        pid2 = fork();

        if (pid2 == 0) {
            int iterations = random() % 30 + 1; // Random number of iterations, 1 to 30
            for (int i = 0; i < iterations; i++) {
                int sleepTime = random() % 10 + 1; // Random sleep time, 1 to 10 seconds
                printf("Child Pid: %d is going to sleep for %d seconds!\n", getpid(), sleepTime);
                sleep(sleepTime);
                printf("Child Pid: %d is awake! Where is my Parent: %d?\n", getpid(), getppid());
            }
            exit(0); 
        } else {
            
            printf("Parent is waiting for children to complete...\n");
            waitpid(pid1, &status, 0);
            printf("Child Pid: %d has completed.\n", pid1);

            waitpid(pid2, &status, 0);
            printf("Child Pid: %d has completed.\n", pid2);

            printf("Parent is done.\n");
        }
    }

    return 0;
}

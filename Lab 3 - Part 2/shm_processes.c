#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define LOOP_COUNT 25

void ParentProcess(int *BankAccount, int *Turn);
void ChildProcess(int *BankAccount, int *Turn);

int main() {
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    // Allocate shared memory for two integers: BankAccount and Turn
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error ***\n");
        exit(1);
    }

    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** shmat error ***\n");
        exit(1);
    }

    // Initialize shared variables
    ShmPTR[0] = 0;  // BankAccount
    ShmPTR[1] = 0;  // Turn

    printf("Shared memory created and initialized...\n");

    pid = fork();
    if (pid < 0) {
        printf("*** fork error ***\n");
        exit(1);
    } else if (pid == 0) {
        // Child process
        ChildProcess(&ShmPTR[0], &ShmPTR[1]);
        exit(0);
    } else {
        // Parent process
        ParentProcess(&ShmPTR[0], &ShmPTR[1]);
    }

    wait(&status);
    shmdt((void *) ShmPTR);
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Shared memory detached and removed.\n");
    return 0;
}

void ParentProcess(int *BankAccount, int *Turn) {
    srand(time(NULL)); 
    for (int i = 0; i < LOOP_COUNT; i++) {
        sleep(rand() % 6); 
        int account = *BankAccount;  
        while (*Turn != 0); 

        if (account <= 100) {
            int balance = rand() % 101;  
            if (balance % 2 == 0) {
                account += balance;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
            } else {
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        } else {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

        *BankAccount = account;  // Update shared BankAccount
        *Turn = 1;  // Set Turn to 1
    }
}

void ChildProcess(int *BankAccount, int *Turn) {
    srand(time(NULL) + 1);  
    for (int i = 0; i < LOOP_COUNT; i++) {
        sleep(rand() % 6); 

        int account = *BankAccount; 
        while (*Turn != 1); 

        int balance = rand() % 51; 
        printf("Poor Student needs $%d\n", balance);

        if (balance <= account) {
            account -= balance;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
        } else {
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        *BankAccount = account;  
        *Turn = 0; 
    }
}

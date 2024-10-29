#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


int *BankAccount;
int *Turn;
int shmid1, shmid2;

void DearOldDad();
void PoorStudent();

int main() {
    srand(time(NULL));

    key_t key1 = ftok("shmfile1", 65);
    key_t key2 = ftok("shmfile2", 66);

    shmid1 = shmget(key1, sizeof(int), 0666 | IPC_CREAT);
    shmid2 = shmget(key2, sizeof(int), 0666 | IPC_CREAT);

    if (shmid1 < 0 || shmid2 < 0) {
        perror("shmget failed");
        exit(1);
    }

    BankAccount = (int *)shmat(shmid1, (void *)0, 0);
    Turn = (int *)shmat(shmid2, (void *)0, 0);

    if (BankAccount == (void *) -1 || Turn == (void *) -1) {
        perror("shmat failed");
        exit(1);
    }

    *BankAccount = 0;
    *Turn = 0;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        shmdt(BankAccount);
        shmctl(shmid1, IPC_RMID, NULL);
        shmdt(Turn);
        shmctl(shmid2, IPC_RMID, NULL);
        exit(1);
    } else if (pid == 0) {
        PoorStudent();
        shmdt(BankAccount);
        shmdt(Turn);
    } else {
        DearOldDad();
        wait(NULL);
        shmdt(BankAccount);
        shmctl(shmid1, IPC_RMID, NULL);
        shmdt(Turn);
        shmctl(shmid2, IPC_RMID, NULL);
    }

    return 0;
}

void DearOldDad() {
    int i;
    for (i = 0; i < 25; i++) {
        sleep(rand() % 6);

        int account = *BankAccount;

        while (*Turn != 0)
            ;

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

        *BankAccount = account;
        *Turn = 1;
    }
}

void PoorStudent() {
    int i;
    for (i = 0; i < 25; i++) {
        sleep(rand() % 6);

        int account = *BankAccount;

        while (*Turn != 1)
            ;

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


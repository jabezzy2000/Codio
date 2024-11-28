#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

void dear_old_dad(sem_t *mutex, int *bankAccount) {
    while (1) {
        sleep(rand() % 5);
        printf("Dear Old Dad: Attempting to Check Balance\n");

        sem_wait(mutex); 
        int localBalance = *bankAccount;
        int randomNum = rand();
        if (randomNum % 2 == 0) {
            if (localBalance < 100) {
                int amount = rand() % 100; 
                if (rand() % 2 == 0) {
                    localBalance += amount;
                    printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
                } else {
                    printf("Dear Old Dad: Doesn't have any money to give\n");
                }
                *bankAccount = localBalance;
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
            }
        } else {
            printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
        }
        sem_post(mutex); 
    }
}

void lovable_mom(sem_t *mutex, int *bankAccount) {
    while (1) {
        sleep(rand() % 10); 
        printf("Lovable Mom: Attempting to Check Balance\n");

        sem_wait(mutex); 
        int localBalance = *bankAccount;
        if (localBalance <= 100) {
            int amount = rand() % 125; 
            localBalance += amount;
            printf("Lovable Mom: Deposits $%d / Balance = $%d\n", amount, localBalance);
            *bankAccount = localBalance;
        } else {
            printf("Lovable Mom: Thinks Student has enough Cash ($%d)\n", localBalance);
        }
        sem_post(mutex); 
    }
}

void poor_student(sem_t *mutex, int *bankAccount) {
    while (1) {
        sleep(rand() % 5); 
        printf("Poor Student: Attempting to Check Balance\n");

        sem_wait(mutex); 
        int localBalance = *bankAccount;
        int randomNum = rand();
        if (randomNum % 2 == 0) {
            int need = rand() % 50; 
            printf("Poor Student needs $%d\n", need);
            if (need <= localBalance) {
                localBalance -= need;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
            }
            *bankAccount = localBalance; 
        } else {
            printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
        }
        sem_post(mutex); 
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_parents> <num_children>\n", argv[0]);
        exit(1);
    }

    int num_parents = atoi(argv[1]);
    int num_children = atoi(argv[2]);

    int fd, zero = 0, *bankAccount;
    sem_t *mutex;


    fd = open("bank_account.txt", O_RDWR | O_CREAT, S_IRWXU);
    write(fd, &zero, sizeof(int)); 
    bankAccount = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if ((mutex = sem_open("bank_semaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("Semaphore initialization failed");
        exit(1);
    }

    srand(time(NULL));


    for (int i = 0; i < num_parents; i++) {
        if (fork() == 0) {
            if (i == 0) {
                dear_old_dad(mutex, bankAccount);
            } else {
                lovable_mom(mutex, bankAccount);
            }
            exit(0);
        }
    }

    for (int i = 0; i < num_children; i++) {
        if (fork() == 0) {
            poor_student(mutex, bankAccount);
            exit(0);
        }
    }

    while (1) {
        sleep(1);
    }

    return 0;
}

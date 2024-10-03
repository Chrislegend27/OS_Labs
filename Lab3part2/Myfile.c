#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void DearOldDad(int []);
void PoorStudent(int []);

int main(int argc, char *argv[]) {
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    // Create a shared memory segment for 2 integers (BankAccount, Turn)
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error ***\n");
        exit(1);
    }
    printf("Server has received a shared memory segment for BankAccount and Turn...\n");

    // Attach shared memory
    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** shmat error ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");

    // Initialize shared variables
    ShmPTR[0] = 0;  // BankAccount
    ShmPTR[1] = 0;  // Turn

    printf("Server is about to fork a child process...\n");
    pid = fork();
    if (pid < 0) {
        printf("*** fork error ***\n");
        exit(1);
    }
    else if (pid == 0) {
        PoorStudent(ShmPTR);
        exit(0);
    } else {
        DearOldDad(ShmPTR);
    }

    wait(&status);
    printf("Server has detected the completion of its child process...\n");

    // Detach and remove shared memory
    shmdt((void *) ShmPTR);
    printf("Server has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server has removed its shared memory...\n");

    printf("Server exits...\n");
    return 0;
}

void DearOldDad(int SharedMem[]) {
    int account, balance;

    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Sleep between 0-5 seconds
        account = SharedMem[0];  // BankAccount

        // Wait until it's the parent's turn (Turn == 0)
        while (SharedMem[1] != 0) {
            // Busy wait
        }

        if (account <= 100) {
            balance = rand() % 101;  // Random amount between 0-100

            if (balance % 2 == 0) {
                account += balance;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
            } else {
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        } else {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

        // Update shared memory and give turn to child
        SharedMem[0] = account;  // Update BankAccount
        SharedMem[1] = 1;        // Set Turn to 1 (child's turn)
    }
}

void PoorStudent(int SharedMem[]) {
    int account, balance;

    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Sleep between 0-5 seconds
        account = SharedMem[0];  // BankAccount

        // Wait until it's the child's turn (Turn == 1)
        while (SharedMem[1] != 1) {
            // Busy wait
        }

        balance = rand() % 51;  // Random amount between 0-50
        printf("Poor Student needs $%d\n", balance);

        if (balance <= account) {
            account -= balance;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
        } else {
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        // Update shared memory and give turn to parent
        SharedMem[0] = account;  // Update BankAccount
        SharedMem[1] = 0;        // Set Turn to 0 (parent's turn)
    }
}

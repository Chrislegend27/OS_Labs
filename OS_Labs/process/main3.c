#include <stdio.h>
#include <stdlib.h>  // For srandom(), random(), exit()
#include <unistd.h>  // For fork(), getpid(), getppid(), sleep()
#include <sys/types.h>
#include <sys/wait.h>  // For wait()
#include <time.h>  // For time()

void ChildProcess() {
    long randomNumber = random() % 30 + 1;  // Random number of iterations between 1 and 30
    int i;
    for (i = 0; i < randomNumber; i++) {
        printf("%d\n", i);
        long sleepTime = random() % 10 + 1;  // Random sleep time between 1 and 10 seconds
        printf("Child Pid: %d is going to sleep!\n", getpid());
        sleep(sleepTime);
        printf("Child Pid: %d is awake!\nWhere is my Parent: %d?\n", getpid(), getppid());
    }
    exit(0);  // Terminate the child process after finishing
}

int main() {
    srandom(time(NULL));  // Seed the random number generator once in the parent process

    pid_t child1 = fork();  // First child process
    if (child1 < 0) {
        // Error in creating the first child process
        perror("fork failed");
        exit(1);
    }
    if (child1 == 0) {
        ChildProcess();  // First child process
    }

    pid_t child2 = fork();  // Second child process
    if (child2 < 0) {
        // Error in creating the second child process
        perror("fork failed");
        exit(1);
    }
    if (child2 == 0) {
        ChildProcess();  // Second child process
    }

    printf("Parent Pid: %d is waiting for children to complete.\n", getpid());

    // Parent process waits for both children to complete
    int status;
    pid_t terminated_child;
    while ((terminated_child = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            printf("Child Pid: %d has completed with exit status %d\n", terminated_child, WEXITSTATUS(status));
        } else {
            printf("Child Pid: %d terminated abnormally\n", terminated_child);
        }
    }

    printf("Parent Pid: %d has finished.\n", getpid());

    return 0;
}

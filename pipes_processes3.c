#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>  // Include this header for waitpid
#include <stdlib.h>

/**
 * Executes the command "cat scores | grep <argument> | sort"
 *
 */

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <grep_argument>\n", argv[0]);
        return 1;
    }

    char *grep_arg = argv[1];

    int pipefd1[2];  // pipe between cat and grep
    int pipefd2[2];  // pipe between grep and sort

    pid_t pid1, pid2, pid3;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", grep_arg, NULL};
    char *sort_args[] = {"sort", NULL};

    // create pipe1
    if (pipe(pipefd1) == -1) {
        perror("pipe1");
        exit(1);
    }

    // create pipe2
    if (pipe(pipefd2) == -1) {
        perror("pipe2");
        exit(1);
    }

    // fork process C (sort)
    pid3 = fork();
    if (pid3 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid3 == 0) {
        // Child process C: executes sort
        // stdin from pipefd2[0]
        // close unused write ends and pipe1
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[1]);

        dup2(pipefd2[0], STDIN_FILENO);
        close(pipefd2[0]);

        execvp("sort", sort_args);
        perror("execvp sort");
        exit(1);
    }

    // fork process B (grep)
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid2 == 0) {
        // Child process B: executes grep
        // stdin from pipefd1[0], stdout to pipefd2[1]
        close(pipefd1[1]);
        close(pipefd2[0]);

        dup2(pipefd1[0], STDIN_FILENO);
        close(pipefd1[0]);

        dup2(pipefd2[1], STDOUT_FILENO);
        close(pipefd2[1]);

        execvp("grep", grep_args);
        perror("execvp grep");
        exit(1);
    }

    // fork process A (cat)
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0) {
        // Child process A: executes cat
        // stdout to pipefd1[1]
        close(pipefd1[0]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        dup2(pipefd1[1], STDOUT_FILENO);
        close(pipefd1[1]);

        execvp("cat", cat_args);
        perror("execvp cat");
        exit(1);
    }

    // Parent process
    // Close all pipe fds
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    // Wait for all child processes to finish
    waitpid(pid1, NULL, 0);  // wait for cat
    waitpid(pid2, NULL, 0);  // wait for grep
    waitpid(pid3, NULL, 0);  // wait for sort

    return 0;
}

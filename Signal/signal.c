/* timer.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int my_signal = 0;
int total_alarms = 0;
time_t start_time; // To track the start time of the program

// Signal handler for SIGALRM
void handler(int signum)
{
    printf("Hello World!\n");
    total_alarms++;
    my_signal = 1;
    alarm(1); // Schedule the next alarm for 1 second later
}

// Signal handler for SIGINT (CTRL-C)
void handle_sigint(int signum)
{
    time_t end_time;
    time(&end_time); // Get the current time
    double total_time = difftime(end_time, start_time); // Calculate the total time in seconds

    printf("\nProgram was executed for %.f seconds.\n", total_time);
    printf("Total alarms received: %d\n", total_alarms);
    
    exit(0); // Exit the program
}

int main(int argc, char *argv[])
{
    // Register signal handlers
    signal(SIGALRM, handler); // Register handler to handle SIGALRM
    signal(SIGINT, handle_sigint); // Register handler to handle SIGINT (CTRL-C)

    // Start the timer and schedule the first alarm
    time(&start_time); // Record the start time of the program
    alarm(1); // Schedule a SIGALRM for 1 second

    while (1)
    {
        if (my_signal)
        {
            printf("Turing was right!\n");
            my_signal = 0;
        }
    } // Busy wait for signal to be delivered

    return 0; // Never reached
}

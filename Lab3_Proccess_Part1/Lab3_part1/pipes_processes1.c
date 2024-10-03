// Modified C program to create a 2-way pipe communication between P1 and P2
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <string.h> 
#include <sys/wait.h> 

int main() 
{ 
    int fd1[2];  // Pipe for P1 to P2 communication 
    int fd2[2];  // Pipe for P2 to P1 communication 

    char fixed_str1[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org"; 
    char input_str[100]; 
    pid_t p; 

    // Create the pipes
    if (pipe(fd1) == -1) 
    { 
        fprintf(stderr, "Pipe Failed"); 
        return 1; 
    } 
    if (pipe(fd2) == -1) 
    { 
        fprintf(stderr, "Pipe Failed"); 
        return 1; 
    } 

    printf("Enter a string to concatenate: ");
    scanf("%s", input_str); 

    p = fork(); 

    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed"); 
        return 1; 
    } 
    else if (p > 0) // Parent process P1
    { 
        // Close unused ends
        close(fd1[0]); // Close reading end of first pipe
        close(fd2[1]); // Close writing end of second pipe

        // Write input string to child via first pipe
        write(fd1[1], input_str, strlen(input_str) + 1); 
        close(fd1[1]); // Close writing end after sending

        // Wait for child process to complete
        wait(NULL); 

        // Read the string sent back by child via second pipe
        char received_str[100];
        read(fd2[0], received_str, sizeof(received_str)); 
        close(fd2[0]); // Close reading end after receiving

        // Concatenate "gobison.org" to the received string
        strcat(received_str, fixed_str2);

        // Print the final concatenated string
        printf("Final concatenated string: %s\n", received_str); 
    } 
    else // Child process P2
    { 
        // Close unused ends
        close(fd1[1]); // Close writing end of first pipe
        close(fd2[0]); // Close reading end of second pipe

        // Read the string sent by parent via first pipe
        char concat_str1[100]; 
        read(fd1[0], concat_str1, sizeof(concat_str1)); 
        close(fd1[0]); // Close reading end after receiving

        // Concatenate "howard.edu" to the received string
        strcat(concat_str1, fixed_str1);

        // Print the concatenated string
        printf("Concatenated string: %s\n", concat_str1);

        // Prompt user for a second input
        char input_str2[100];
        printf("Child: Enter a second string to send back: ");
        scanf("%s", input_str2);

        // Send the second input string back to parent via second pipe
        write(fd2[1], input_str2, strlen(input_str2) + 1); 
        close(fd2[1]); // Close writing end after sending

        exit(0); 
    } 

    return 0; 
} 

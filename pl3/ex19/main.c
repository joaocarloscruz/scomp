#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define P_READ 0
#define P_WRITE 1

int main() {
    int pipe1[2], pipe2[2];

    // Create two pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // First child process: ls -la
    if (fork() == 0) {
        // Redirect stdout to the write end of the first pipe
        dup2(pipe1[P_WRITE], STDOUT_FILENO);
        
        // Close all pipe ends in the child
        close(pipe1[P_READ]);
        close(pipe1[P_WRITE]);
        close(pipe2[P_READ]);
        close(pipe2[P_WRITE]);
        
        // Execute ls -la
        execlp("ls", "ls", "-la", NULL);
        perror("execlp ls"); // This only runs if execlp fails
        exit(EXIT_FAILURE);
    }

    // Second child process: sort
    if (fork() == 0) {
        // Redirect stdin to the read end of the first pipe
        dup2(pipe1[P_READ], STDIN_FILENO);
        // Redirect stdout to the write end of the second pipe
        dup2(pipe2[P_WRITE], STDOUT_FILENO);

        // Close all pipe ends
        close(pipe1[P_READ]);
        close(pipe1[P_WRITE]);
        close(pipe2[P_READ]);
        close(pipe2[P_WRITE]);

        // Execute sort
        execlp("sort", "sort", NULL);
        perror("execlp sort");
        exit(EXIT_FAILURE);
    }

    // Third child process: wc -l
    if (fork() == 0) {
        // Redirect stdin to the read end of the second pipe
        dup2(pipe2[P_READ], STDIN_FILENO);

        // Close all pipe ends
        close(pipe1[P_READ]);
        close(pipe1[P_WRITE]);
        close(pipe2[P_READ]);
        close(pipe2[P_WRITE]);

        // Execute wc -l
        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc");
        exit(EXIT_FAILURE);
    }

    // Parent process
    // Close all pipe ends in the parent, otherwise the pipes will remain open
    close(pipe1[P_READ]);
    close(pipe1[P_WRITE]);
    close(pipe2[P_READ]);
    close(pipe2[P_WRITE]);

    // Wait for all three child processes to finish
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}

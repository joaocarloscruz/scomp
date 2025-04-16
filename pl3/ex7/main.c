#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h> // For generating random numbers

#define NUM_CHILDREN 10

typedef struct {
    char message[5]; // "Win!"
    int round;
} GameData;

int main() {
    int pipe_fd[2];
    pid_t child_pids[NUM_CHILDREN];
    int child_exit_status[NUM_CHILDREN];

    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        return 1;
    }
    // Seed the random number generator
    srand(time(NULL) ^ (getpid() << 16)); // Use current time and process ID for better randomness

    // Create child processes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Fork failed");
            // Clean up any already created children
            for (int j = 0; j < i; j++) {
                kill(child_pids[j], SIGTERM); // Or another appropriate signal
            }
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            return 1;
        } else if (pid == 0) { // Child process
            close(pipe_fd[1]); // Close write end of the pipe

            // Introduce a small random delay
            usleep(rand() % 500000); // Wait for a random time between 0 and 0.5 seconds

            GameData data;
            ssize_t bytes_read;

            while (1) {
                bytes_read = read(pipe_fd[0], &data, sizeof(GameData));
                if (bytes_read == sizeof(GameData)) {
                    printf("Child process %d received: %s (Round %d)\n", getpid(), data.message, data.round);
                    close(pipe_fd[0]);
                    exit(data.round);
                } else if (bytes_read == 0) {
                    // Parent closed the pipe, no more messages
                    close(pipe_fd[0]);
                    exit(0); // Or some other appropriate exit code
                } else if (bytes_read == -1) {
                    perror("Error reading from pipe");
                    close(pipe_fd[0]);
                    exit(1);
                }
                usleep(10000); // 10 milliseconds
            }
        } else { // Parent process
            child_pids[i] = pid;
        }
    }

    close(pipe_fd[0]); // Close read end of the pipe

    GameData data_to_send;
    strcpy(data_to_send.message, "Win!");

    // Parent writes to the pipe every 2 seconds
    for (int round = 1; round <= 10; round++) {
        data_to_send.round = round;
        printf("Parent sending message for round %d...\n", round);
        if (write(pipe_fd[1], &data_to_send, sizeof(GameData)) == -1) {
            perror("Error writing to pipe");
            break;
        }
        sleep(2);
    }

    close(pipe_fd[1]); // Close write end after sending all messages

    // Parent waits for all child processes to terminate and gets their exit status
    printf("\nWaiting for child processes to finish...\n");
    for (int i = 0; i < NUM_CHILDREN; i++) {
        int status;
        waitpid(child_pids[i], &status, 0);
        if (WIFEXITED(status)) {
            child_exit_status[i] = WEXITSTATUS(status);
        } else {
            child_exit_status[i] = -1; // Indicate an error or abnormal termination
        }
    }

    // Parent prints the round in which each child gained access
    printf("\n--- Race Results ---\n");
    for (int i = 0; i < NUM_CHILDREN; i++) {
        printf("Child process %d finished in round: %d\n", child_pids[i], child_exit_status[i]);
    }

    return 0;
}

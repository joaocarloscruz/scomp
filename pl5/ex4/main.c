#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#define MAX_LINES 10
#define MAX_LEN 256
#define SEM_NAME "/pl5ex4_semaphore"

// Helper function to ensure the semaphore is released before exiting
void cleanup_and_exit(sem_t *sem, int exit_code) {
    if (sem != SEM_FAILED) {
        sem_post(sem);
        sem_close(sem);
    }
    exit(exit_code);
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc == 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <filename> [-d <line_number>]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* filename = argv[1];
    int delete_mode = 0;
    int line_to_delete = -1;

    // Check for delete flag
    if (argc == 4) {
        if (strcmp(argv[2], "-d") == 0) {
            delete_mode = 1;
            line_to_delete = atoi(argv[3]);
        } else {
            fprintf(stderr, "Invalid option: %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    sem_unlink(SEM_NAME);

    sem_t* sem = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open error");
        exit(EXIT_FAILURE);
    }

    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime error");
        sem_close(sem);
        exit(EXIT_FAILURE);
    }
    ts.tv_sec += 10;

    printf("Process %d trying to acquire semaphore...\n", getpid());
    if (sem_timedwait(sem, &ts) == -1) {
        if (errno == ETIMEDOUT) {
            fprintf(stderr, "Process %d timed out waiting for semaphore after 10 seconds.\n", getpid());
        } else {
            perror("sem_timedwait error");
        }
        sem_close(sem);
        exit(EXIT_FAILURE);
    }
    printf("Process %d acquired the semaphore.\n", getpid());


    FILE* fptr;
    char lines[MAX_LINES][MAX_LEN];
    int line_count = 0;

    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("File '%s' not found. It will be created.\n", filename);
    } else {
        while (line_count < MAX_LINES && fgets(lines[line_count], sizeof(lines[0]), fptr)) {
            line_count++;
        }
        fclose(fptr);
    }

    if (delete_mode) {
        printf("Attempting to delete line %d...\n", line_to_delete);
        if (line_to_delete < 1 || line_to_delete > line_count) {
            fprintf(stderr, "Error: Line %d is out of bounds. File has %d lines.\n", line_to_delete, line_count);
            cleanup_and_exit(sem, EXIT_FAILURE);
        }

        fptr = fopen(filename, "w"); 
        if (fptr == NULL) {
            perror("Error opening file for writing");
            cleanup_and_exit(sem, EXIT_FAILURE);
        }

        for (int i = 0; i < line_count; i++) {
            if (i + 1 != line_to_delete) {
                fputs(lines[i], fptr);
            }
        }
        fclose(fptr);
        printf("Successfully deleted line %d.\n", line_to_delete);

    } else { // normal mode
        printf("Attempting to add a line...\n");
        if (line_count >= MAX_LINES) {
            fprintf(stderr, "Error: File is full. Cannot add more than %d lines.\n", MAX_LINES);
            cleanup_and_exit(sem, EXIT_FAILURE);
        }

        fptr = fopen(filename, "a"); 
        if (fptr == NULL) {
            perror("Error opening file for appending");
            cleanup_and_exit(sem, EXIT_FAILURE);
        }
        fprintf(fptr, "I am process [%d]\n", getpid());
        fclose(fptr);
        printf("Successfully added a line.\n");
    }


    printf("Process %d releasing semaphore.\n", getpid());
    sem_post(sem);
    sem_close(sem);

    return 0;
}

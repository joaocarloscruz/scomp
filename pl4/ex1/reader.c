#include "defs.h"

#include <sys/mman.h>   // For mmap, munmap, shm_open, shm_unlink, MAP_FAILED
#include <sys/stat.h>   // For mode constants (like S_IRUSR, S_IWUSR)
#include <fcntl.h>      // For O_* constants (O_CREAT, O_RDWR, O_RDONLY)
#include <unistd.h>     // For ftruncate, close, sleep (optional for reader)
#include <stdio.h>      // For printf, perror, fgets
#include <stdlib.h>     // For exit, atoi, EXIT_FAILURE
#include <string.h>     // For strcpy, strlen

int main() {
    int fd;
    SharedData *shared_data;
    int data_processed = 0; // Flag to ensure we only print once

    printf("Reader process started. Waiting for shared memory '%s'...\n", SHM_NAME);

    // 1. Open the existing shared memory object
    // O_RDONLY: Open for read-only access
    // Permissions (0660) are less critical here but good practice
    fd = shm_open(SHM_NAME, O_RDONLY, 0660);
    if (fd == -1) {
        perror("shm_open failed - writer might not have run yet?");
        exit(EXIT_FAILURE);
    }

    // 2. Map the shared memory object into the process's address space
    // PROT_READ: Memory protection (allow reading only)
    // MAP_SHARED: Required for shared memory view
    shared_data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        // Should we unlink if mmap fails? Maybe not, if other readers exist.
        exit(EXIT_FAILURE);
    }

    // Successfully mapped

    printf("Shared memory attached. Waiting for data available signal...\n");

    // 3. Wait for the writer to signal that data is available
    while (data_processed == 0) {
        // Check the flag written by the writer
        if (shared_data->data_available == 1) {
            // 4. Read and print the data
            printf("\n--- Data Received ---\n");
            printf("Student Number : %d\n", shared_data->number);
            printf("Student Name   : %s\n", shared_data->name);
            printf("Student Address: %s\n", shared_data->address);
            printf("---------------------\n");
            data_processed = 1; // Mark data as processed to exit the loop
        } else {
            // Optional: Add a small delay to prevent high CPU usage (polling)
            // This is better than pure busy-waiting.
            // For true synchronization, use semaphores or condition variables.
             //printf("."); fflush(stdout); // Indicate waiting
             sleep(1); // Wait for 1 second before checking again
        }
    }

    // 5. Cleanup
    printf("Data read. Cleaning up shared memory resources...\n");
    if (munmap(shared_data, sizeof(SharedData)) == -1) {
        perror("munmap failed");
    }
    if (close(fd) == -1) {
        perror("close failed");
    }

    // The reader (or the last process to use the segment) should unlink it
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink failed");
        // This might happen if another process already unlinked it, often not critical
    }

    printf("Reader process finished.\n");
    return 0;
}
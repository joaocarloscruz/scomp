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
    char buffer[100]; // Input buffer

    // 1. Create or open the shared memory object
    // O_CREAT: Create if it doesn't exist
    // O_RDWR: Open for read/write access
    // 0660: Permissions (user read/write, group read/write)
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0660);
    if (fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // 2. Set the size of the shared memory object
    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        perror("ftruncate failed");
        close(fd); // Close the descriptor before exiting
        shm_unlink(SHM_NAME); // Clean up the created object on failure
        exit(EXIT_FAILURE);
    }

    // 3. Map the shared memory object into the process's address space
    // PROT_READ | PROT_WRITE: Memory protection (allow reading and writing)
    // MAP_SHARED: Changes are shared with other processes mapping the same object
    shared_data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        shm_unlink(SHM_NAME); // Clean up
        exit(EXIT_FAILURE);
    }

    // Successfully mapped, now we can access shared_data like a regular pointer

    // 4. Initialize the data_available flag to 0 (data not ready yet)
    shared_data->data_available = 0;

    // 5. Get student data from the user
    printf("Enter Student number: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        fprintf(stderr, "Error reading student number.\n");
        // Cleanup before exit
        munmap(shared_data, sizeof(SharedData));
        close(fd);
        // Consider not unlinking here, let reader or cleanup handle it
        exit(EXIT_FAILURE);
    }
    shared_data->number = atoi(buffer); // Convert string to integer

    printf("Enter Student name: ");
    if (fgets(buffer, 50, stdin) == NULL) { // Use size consistent with shared_data->name
         fprintf(stderr, "Error reading student name.\n");
         munmap(shared_data, sizeof(SharedData));
         close(fd);
         exit(EXIT_FAILURE);
    }
    // Remove trailing newline if present
    buffer[strcspn(buffer, "\n")] = '\0'; // Safer than strlen-1 check
    strcpy(shared_data->name, buffer);

    printf("Enter Student address: ");
     if (fgets(buffer, 100, stdin) == NULL) { // Use size consistent with shared_data->address
         fprintf(stderr, "Error reading student address.\n");
         munmap(shared_data, sizeof(SharedData));
         close(fd);
         exit(EXIT_FAILURE);
    }
    // Remove trailing newline if present
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(shared_data->address, buffer);

    // 6. Signal that data is now available
    printf("\nMaking data available...\n");
    shared_data->data_available = 1;

    printf("Data written to shared memory.\n");

    // 7. Cleanup (Writer typically doesn't unlink, lets the reader or last process do it)
    if (munmap(shared_data, sizeof(SharedData)) == -1) {
        perror("munmap failed");
        // continue cleanup
    }
    if (close(fd) == -1) {
        perror("close failed");
    }

    return 0;
}
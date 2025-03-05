#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILDREN 6
#define NUMBERS_PER_CHILD 200000

void child_process(int child_num) {
    // Calculate start and end numbers based on child number
    long start = (child_num - 1) * NUMBERS_PER_CHILD + 1;
    long end = child_num * NUMBERS_PER_CHILD;
    
    // Print numbers for this child
    for (long i = start; i <= end; i++) {
        printf("Child %d: %ld\n", child_num, i);
    }
    
    exit(0);
}

int main() {
    pid_t pids[NUM_CHILDREN];
    int i;

    for (i = 0; i < NUM_CHILDREN; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Fork failed");
            exit(1);
        }
        else if (pids[i] == 0) {
            child_process(i + 1);
        }
    }

    for (i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    printf("All child processes have completed.\n");
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define NUM_THREADS 5
#define CHUNK_SIZE (ARRAY_SIZE / NUM_THREADS)

int array[ARRAY_SIZE];
int target_number;

void *search_chunk(void *arg) {
    int *thread_num_ptr = (int *)arg;
    int thread_num = *thread_num_ptr;

    int start_index = (thread_num - 1) * CHUNK_SIZE;
    int end_index = start_index + CHUNK_SIZE;

    printf("Thread %d searching from index %d to %d.\n", thread_num, start_index, end_index - 1);

    for (int i = start_index; i < end_index; i++) {
        if (array[i] == target_number) {
            printf("Thread %d found the number at position %d.\n", thread_num, i);
            return (void *)thread_num_ptr;
        }
    }

    free(thread_num_ptr);
    return NULL;
}

void populate_array() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i;     
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int j = rand() % ARRAY_SIZE;
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main() {
    pthread_t threads[NUM_THREADS];
    void *exit_codes[NUM_THREADS];

    srand(time(NULL));

    populate_array();

    // Choose a random number to search for from the array
    target_number = array[rand() % ARRAY_SIZE];
    printf("Main thread: The number to search for is %d.\n\n", target_number);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        int *thread_num_ptr = (int *)malloc(sizeof(int));
        if (thread_num_ptr == NULL) {
            perror("Failed to allocate memory for thread argument");
            exit(EXIT_FAILURE);
        }
        *thread_num_ptr = i + 1;

        if (pthread_create(&threads[i], NULL, search_chunk, (void *)thread_num_ptr) != 0) {
            perror("Failed to create the thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to complete and collect their exit codes
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], &exit_codes[i]) != 0) {
            perror("Failed to join the thread");
            exit(EXIT_FAILURE);
        }
    }

    printf("\nMain thread: All threads have finished.\n");

    // Find which thread found the number
    int *successful_thread_num_ptr = NULL;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (exit_codes[i] != NULL) {
            successful_thread_num_ptr = (int *)exit_codes[i];
            break; 
        }
    }

    if (successful_thread_num_ptr != NULL) {
        printf("Main thread: Thread %d found the target number.\n", *successful_thread_num_ptr);
        free(successful_thread_num_ptr);
    } else {
        printf("Main thread: The target number was not found in the array.\n");
    }

    return 0;
}

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 6

// Global variables for the barrier
pthread_mutex_t mutex;
sem_t *barrier_sem;
int count = 0;

// Functions representing thread actions
void eat_and_drink(int id) {
    printf("Thread %d is eating and drinking.\n", id);
    sleep(1); // Simulate work
}

void buy_chips(int id) {
    printf("Thread %d is buying chips...\n", id);
    sleep(rand() % 2); // Simulate work
}

void buy_beer(int id) {
    printf("Thread %d is buying beer...\n", id);
    sleep(rand() % 2); // Simulate work
}

// The single function all threads will execute
void* worker_thread(void* args) {
    int id = *((int*)args);

    // Step 1: Randomly perform one of the initial tasks
    if (rand() % 2 == 0) {
        buy_beer(id);
    } else {
        buy_chips(id);
    }

    // --- Barrier Synchronization ---

    // Lock the mutex to safely update the counter
    pthread_mutex_lock(&mutex);
    count++;

    // Check if this is the last thread to arrive
    if (count == NUM_THREADS) {
        // This is the last thread. It must release the others.
        // It unlocks the mutex first.
        pthread_mutex_unlock(&mutex);

        // Then it posts to the semaphore for each waiting thread.
        printf("Thread %d was the last to arrive. Opening the gate!\n", id);
        for (int i = 0; i < NUM_THREADS - 1; i++) {
            sem_post(barrier_sem);
        }
    } else {
        // This is not the last thread. It must wait.
        // It unlocks the mutex BEFORE waiting to prevent deadlock.
        printf("Thread %d is waiting at the barrier (%d/%d arrived).\n", id, count, NUM_THREADS);
        pthread_mutex_unlock(&mutex);

        // sem_wait() will pause the thread until the last thread calls sem_post()
        sem_wait(barrier_sem);
    }

    // Step 2: All threads are synchronized and execute this part together.
    printf(">>> Thread %d passed the barrier! <<<\n", id);
    eat_and_drink(id);

    return NULL;
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Clean up any previous semaphore instances
    sem_unlink("/barrier_sem");
    
    // Initialize the mutex
    pthread_mutex_init(&mutex, NULL);

    // Initialize the semaphore with a value of 0
    barrier_sem = sem_open("/barrier_sem", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (barrier_sem == SEM_FAILED) {
        perror("Failed to open semaphore for barrier");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    printf("Creating %d threads...\n", NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads have finished.\n");

    // Clean up resources
    pthread_mutex_destroy(&mutex);
    sem_close(barrier_sem);
    sem_unlink("/barrier_sem");

    return 0;
}
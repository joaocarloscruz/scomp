#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

void* thread_func(void* arg) {
    int max = *((int*) arg);

    printf("Searching for prime numbers until %d...\n", max);

    if (max < 2) {
        printf("No prime numbers to find up to %d.\n", max);
        pthread_exit(NULL);
    }

    int* isPrime = (int*) malloc((max + 1) * sizeof(int));
    if (isPrime == NULL) {
        perror("Failed to allocate memory for isPrime array");
        pthread_exit(NULL);
    }

    for (int i = 0; i <= max; i++) {
        isPrime[i] = 1;
    }

    isPrime[0] = 0;
    isPrime[1] = 0;

    for (int p = 2; p * p <= max; p++) {
        if (isPrime[p] == 1) {
            for (int i = p * p; i <= max; i += p) {
                isPrime[i] = 0;
            }
        }
    }

    printf("Prime numbers up to %d:\n", max);
    for (int i = 2; i <= max; i++) {
        if (isPrime[i] == 1) {
            printf("%d ", i);
        }
    }
    printf("\n");

    free(isPrime);
    return NULL;
}

int main() {
    int value;
    pthread_t thread;

    printf("Insert the value until which you want to check for prime numbers: ");
    scanf("%d", &value);

    if (pthread_create(&thread, NULL, thread_func, (void*)&value) != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    pthread_join(thread, NULL);

    return 0;
}

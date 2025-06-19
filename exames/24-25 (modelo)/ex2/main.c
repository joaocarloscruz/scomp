#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define NUM_WRITERS 3
#define NUM_READERS 3

pthread_mutex_t mutex;
sem_t semaphore;

int read_count = 0;

//shared data structure
typedef struct{
    char string1[50];
    char string2[50];
} SharedData;
SharedData* shared;

void* reader_func(void* args){
    int id = (int) args;
    while(1){
        pthread_mutex_lock(&mutex);
        read_count++;
        if(read_count == 1){
            sem_wait(&semaphore);
        }
        pthread_mutex_unlock(&mutex);

        // reading logic
        printf("Reader %d is reading...\n", id);
        printf("String 1: %s\n", shared->string1);
        printf("String 2: %s\n", shared->string2);
        printf("Reader %d finished\n", id);

        pthread_mutex_lock(&mutex);
        read_count--;
        if(read_count == 0){
            sem_post(&semaphore);
        }
        pthread_mutex_unlock(&mutex);
        
    }
    pthread_exit(NULL);
}

void* writer_func(void* args){
    int id = (int) args;
    while(1){
        sem_wait(&semaphore);
        printf("Writer %d is writing...\n", id);

        time_t now = time(NULL);
        char* time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0';

        // id in string 1
        sprintf(shared->string1, "%ld", id);
        // time in string 2
        sprintf(shared->string2, "%s", time_str);

        printf("Writer %d fnished\n",id);

        sem_post(&semaphore);
    }
    pthread_exit(NULL);
}

int main(){
    shared = (SharedData*) malloc(sizeof(SharedData));
    if(shared == NULL){
        perror("failed creating shared memory");
        exit(EXIT_FAILURE);
    }

    //create one mutex and one semaphore
    if(pthread_mutex_init(&mutex, NULL) != 0){
        perror("failed creating mutex");
        exit(EXIT_FAILURE);
    }
    if(sem_init(&semaphore, 0, 1) != 0){
        perror("failed creating semaphore");
        exit(EXIT_FAILURE);
    }
    
    // create threads using for loops
    pthread_t reader_threads[NUM_READERS];
    for (int i = 0; i < NUM_READERS; ++i) {
        pthread_create(&reader_threads[i], NULL, reader_func, (void*) i);
    }

    pthread_t writer_threads[NUM_WRITERS];
    for (int i = 0; i < NUM_WRITERS; ++i) {
        pthread_create(&writer_threads[i], NULL, writer_func, (void*) i);
    }
    
    //wait for threads to finish
    for (int i = 0; i < NUM_READERS; ++i) {
        pthread_join(reader_threads[i], NULL);
    }
    for (int i = 0; i < NUM_WRITERS; ++i) {
        pthread_join(writer_threads[i], NULL);
    }

    //destroy
    pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);

    free(shared);
    return 0;
}

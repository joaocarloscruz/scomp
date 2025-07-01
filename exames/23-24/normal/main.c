#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 4
#define PRINTER_NUMBER 3
#define CLIENT_NUMBER 5

typedef struct{
    int id;
    int client_id;
    char filename[50];
} PrinterJob;

PrinterJob printing_queue[BUFFER_SIZE];
int head = 0;
int tail = 0;
int current_size = 0;

pthread_mutex_t mutex;
pthread_cond_t full_queue;
pthread_cond_t empty_queue;



void* client_function(void* args){
    int client_id = *(int*)args;
    free(args); // Free the allocated memory for the client ID
    
    char* filename = "printing.pdf";
    while(1){
        pthread_mutex_lock(&mutex);
        while(current_size == BUFFER_SIZE){ // queue is full
            pthread_cond_wait(&full_queue, &mutex);
        }

        PrinterJob job;
        job.id = head;
        job.client_id = client_id;
        strcpy(job.filename, filename);
        printing_queue[head] = job;
        head = (head + 1) % BUFFER_SIZE;
        current_size++;

        printf("[CLIENT %d]: Sent job for the printer\n", client_id);
        pthread_cond_signal(&empty_queue); // queue not empty
        pthread_mutex_unlock(&mutex);

        sleep(2);
    }
    return NULL;
}

void* printer_function(void* args){
    int printer_id = *(int*)args;
    free(args); // Free the allocated memory for the printer ID
    while(1){
        pthread_mutex_lock(&mutex);
        while(head == tail){ // qeuue is empty
                pthread_cond_wait(&empty_queue, &mutex);
        }

        PrinterJob job = printing_queue[tail];
        tail = (tail + 1) % BUFFER_SIZE;
        current_size--;

        pthread_cond_signal(&full_queue); // queue not full

        pthread_mutex_unlock(&mutex);
        printf("[PRINTER %d]: Printing job for client %d\n", printer_id, job.client_id);
        fflush(stdout);
        sleep(5);
        printf("[PRINTER %d]: Finished printing job for client %d\n", printer_id, job.client_id);
        fflush(stdout);
    }   
    return NULL;
}

int main(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full_queue, NULL);
    pthread_cond_init(&empty_queue, NULL);

    pthread_t client_threads[CLIENT_NUMBER];
    pthread_t printer_threads[PRINTER_NUMBER];

    for(int i = 0; i < PRINTER_NUMBER; i++){
        int* id = malloc(sizeof(int));
        *id = i;
        if(pthread_create(&printer_threads[i], NULL, printer_function, id) != 0){
            perror("pthread_create error");
            exit(1);
        }
    }

    for(int i = 0; i < CLIENT_NUMBER; i++){
        int* id = malloc(sizeof(int));
        *id = i;
        if(pthread_create(&client_threads[i], NULL, client_function, id) != 0){
            perror("pthread_create error");
            exit(1);
        }

    }
    

    for(int i = 0; i < PRINTER_NUMBER; i++){
        if(pthread_join(printer_threads[i], NULL) != 0){
            perror("pthread_join error");
            exit(1);
        }
    }

    for(int i = 0; i < CLIENT_NUMBER; i++){
        if(pthread_join(client_threads[i], NULL) != 0){
            perror("pthread_join error");
            exit(1);
        }
    }





    return 0;
}
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>    
#include <sys/stat.h> 
#include <unistd.h>

sem_t* beer;
sem_t* chips;

void eat_and_drink(int i){
    
    if(i == 1){
        printf("Eating those chips\n");
    } else {
        printf("Chugging that beer\n");
    }
}

void buy_chips(){
    printf("chips bough\n");
    sem_post(chips);
}

void buy_beer(){
    printf("beer bought\n");
    sem_post(beer);
}

void* thread_1_func(void* args){
    buy_chips();

    sem_wait(beer);
    eat_and_drink(1);
    return NULL;
}

void* thread_2_func(void* args){
    buy_beer();

    sem_wait(chips);
    eat_and_drink(2);
    return NULL;
}



int main(){

    sem_unlink("/sem_chips");
    sem_unlink("/sem_beer");

    pthread_t t1, t2;

    beer = sem_open("/sem_beer", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (beer == SEM_FAILED) {
        perror("Failed to open beer semaphore");
        exit(EXIT_FAILURE);
    }
    chips = sem_open("/sem_chips", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (chips == SEM_FAILED) {
        perror("Failed to open chips semaphore");
        exit(EXIT_FAILURE);
    }

    pthread_create(&t1, NULL, thread_1_func, NULL);
    pthread_create(&t2, NULL, thread_2_func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_close(chips);
    sem_close(beer);
    sem_unlink("/sem_chips");
    sem_unlink("/sem_beer");

    return 0;
}

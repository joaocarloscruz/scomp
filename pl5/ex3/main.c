#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>


int main(int argc, char* argv[]){
    if(argc != 2){
        perror("no filename given.");
        exit(EXIT_FAILURE);
    }

    FILE* fptr;
    char* filename = argv[1];
    sem_unlink("/semaphore");
    
    sem_t* sem = sem_open("/semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1);
    if(sem == SEM_FAILED){
        perror("semaphore error");
        exit(EXIT_FAILURE);
    }

    printf("Process %d trying to acquire semaphore...\n", getpid());
    if(sem_wait(sem) == -1){
        perror("semaphore error");
        exit(EXIT_FAILURE);
    } 
    printf("Process %d acquired the semaphore.\n", getpid());
    
    printf("Opening file...\n");
    fptr = fopen(filename, "a");
    if (fptr == NULL) {
        perror("Error opening file");
        sem_post(sem);
        sem_close(sem);
        exit(EXIT_FAILURE);
    }

    pid_t pid = getpid();

    printf("writing to file\n");

    fprintf(fptr, "I am process [%d]\n", pid);
    fflush(fptr);

    if (fclose(fptr) != 0) {
        perror("fclose failed");
        sem_post(sem);
        sem_close(sem);
        exit(EXIT_FAILURE);
    }

    printf("asleep...\n");
    sleep(2);

    if(sem_post(sem) == -1){
        perror("semaphore error");
        exit(EXIT_FAILURE);
    }
    if (sem_close(sem) == -1) {
         perror("error closing semaphore");
         exit(EXIT_FAILURE);
    }


    return 0;
}

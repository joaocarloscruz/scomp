#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5
#define NUM_PER_PROCESS 200
#define NUM_TO_WRITE (NUM_PROCESSES * NUM_PER_PROCESS)


// keep same structure as exercise 1 but add a waitpid on the parent process for every iteration.
// This makes it so the parent immediately waits for the child that was created to finish the process, before continuing to the next child
int main(){
    pid_t child_pid[NUM_PROCESSES];
    FILE* fptr;
    fptr = fopen("numbers.txt", "w");
    if(fptr == NULL){
        printf("Error opening file\n");
        exit(1);
    }
    sem_t *sem;
    sem = sem_open("/semaphore", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1); 
    if(sem == SEM_FAILED){
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < NUM_PROCESSES; i++){
        child_pid[i] = fork();
        if(child_pid[i] < 0){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(child_pid[i] == 0){
            int start = i * NUM_PER_PROCESS;
            int end = start + NUM_PER_PROCESS;
            sem_wait(sem);
            for(int j = start; j < end; j++){ 
                fprintf(fptr, "%d\n", j);
            }
            sem_post(sem);
            sem_close(sem);
            exit(EXIT_SUCCESS);
        }
        waitpid(child_pid[i], NULL, 0 );
    }

    for(int i = 0; i < NUM_PROCESSES; i++){
        waitpid(child_pid[i], NULL, 0);
    }

    sem_close(sem);
    sem_unlink("/semaphore");

    fptr = fopen("numbers.txt", "r");
    int len = 256;
    char line[len];
    int number;
    printf("Printing values from text file... \n");
    while(fgets(line, len, fptr )){
        number = atoi(line);
        printf("Read number: %d\n", number);
    }
    fclose(fptr);



    return 0;
}

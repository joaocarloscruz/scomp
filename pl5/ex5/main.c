#include <stdio.h>     
#include <stdlib.h>     
#include <unistd.h>    
#include <sys/wait.h>   
#include <semaphore.h> 
#include <fcntl.h>      

#define SEM_NAME "/sync_semaphore"

int main() {
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);

    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Create a new process
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        sem_close(sem);
        sem_unlink(SEM_NAME);
        exit(EXIT_FAILURE);

    } else if (pid == 0) {
	// child process
        sem_wait(sem); // wait for the parent process to increment sem
        
        printf("I am the child\n");
        
        // clean up
        sem_close(sem);
        exit(EXIT_SUCCESS);

    } else {
        // parent process

        printf("I am the parent\n");

	sem_post(sem); // increment semaphore
        
        wait(NULL);

        // clean up
        sem_close(sem);
        sem_unlink(SEM_NAME);
    }

    return 0;
}





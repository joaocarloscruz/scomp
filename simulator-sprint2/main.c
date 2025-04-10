#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>


#define P_READ 0
#define P_WRITE 1
#define NUM_DRONES 50
#define EXIT_ERROR -1

int main(){
    pid_t drone_pid[NUM_DRONES];
    pid_t pid;
    int fd[2];

    // creation of drone processes
    for(int i = 0; i < NUM_DRONES; i++){

        if(pipe(fd) == - 1){
            perror("pipe failed");
            exit(EXIT_ERROR);
        }
        pid = fork();
        if(pid < 0){
            perror("fork failed");
            exit(EXIT_ERROR);
        }
        if(pid == 0){
            // Child Process
            close(fd[P_READ]);
            // script 
            // ...

            //exec

        } else {
            // Parent process
            close(fd[P_WRITE]);
            drone_pid[i] = pid;
        }


    }

    printf("All drones created\n");

    // waiting for all drones to finish their "movement"
    for (int i = 0; i < NUM_DRONES; i++) {
        waitpid(drone_pid[i], NULL, 0);
    }

    printf("All drones finished\n");
   


    return 0;

}
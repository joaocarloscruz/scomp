#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define P_READ 0
#define P_WRITE 1
#define DRONE_PROGRAM "./drone_program"
#define SCRIPT_DIR "drone_scripts"
#define NUM_DRONES 5
#define ARGUMENT_SIZE 50
#define EXIT_ERROR -1

int main(){
    pid_t drone_pid[NUM_DRONES];
    pid_t pid;
    int fd[NUM_DRONES][2];

    // creation of drone processes
    for(int i = 0; i < NUM_DRONES; i++){

        if(pipe(fd[i]) == - 1){
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
            close(fd[i][P_READ]);
            

            // 1 - drone program | 2 - drone id | 3 - pipe fd | 4 - script path 
            char drone_id[ARGUMENT_SIZE];
            sprintf(drone_id, "%d", i);
            char pipe_fd[ARGUMENT_SIZE];
            sprintf(pipe_fd, "%d", fd[i][P_WRITE]);
            char script_path[ARGUMENT_SIZE];
            sprintf(script_path, "%s/drone%d_script.csv", SCRIPT_DIR, i);
            
            char* drone_argv[5];
            drone_argv[0] = DRONE_PROGRAM;
            drone_argv[1] = drone_id;
            drone_argv[2] = pipe_fd;
            drone_argv[3] = script_path;
            drone_argv[4] = NULL;

            printf("[Child PID %d]: %s %s %s %s\n", getpid(), drone_argv[0], drone_argv[1], drone_argv[2], drone_argv[3]);

            execvp(drone_argv[0], drone_argv);

            // exec fails
            fprintf(stderr, "[Child PID %d]: Failed to execute '%s'",getpid(), DRONE_PROGRAM);
            
            close(fd[i][P_WRITE]);
            exit(EXIT_ERROR); 


        } else {
            // Parent process
            close(fd[i][P_WRITE]);
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
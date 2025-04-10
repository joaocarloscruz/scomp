#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> 
#include <signal.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define SIMULATION_STEPS 10    
#define BUFFER_SIZE 512
#define MAX_SCRIPT_LINE_LEN 100
#define MAX_POSITIONS SIMULATION_STEPS 

typedef struct {
    int x;
    int y;
    int z;
    int time_step;
} Position;

void signal_handler(){

}


int main(int argc, char *argv[]){

    // parse the arguments 
    // ...

    // parse script file
    Position drone_positions[MAX_POSITIONS];
    // ...

    
    // sigaction
    struct sigaction_sa sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    // simulation
    char buffer[BUFFER_SIZE];
    for(int i = 0; i < SIMULATION_STEPS; i++){
        int len = snprintf(buffer, BUFFER_SIZE, "%d:%d:%d,%d,%d",drone_id, step,positions[step].x,positions[step].y,positions[step].z);
        íf(write(fd[P_WRITE], buffer, len) == -1){
            perror("write failed");
            exit(EXIT_ERROR);
        }
        
    }

    close(fd[P_WRITE]);
    

    return 0;

}


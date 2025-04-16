#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> 
#include <signal.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>


#define P_READ 0
#define P_WRITE 1
#define SIMULATION_STEPS 10    
#define BUFFER_SIZE 512
#define MAX_LINE_LENGTH 100
#define MAX_POSITIONS SIMULATION_STEPS 
#define EXIT_ERROR -1


typedef struct {
    int x;
    int y;
    int z;
    int time_step;
} Position;

void signal_handler(int signum){

}


int main(int argc, char *argv[]){
    int drone_id;
    int pipe_write;
    const char *script_file_path;
    

    // parse the arguments 
    if (argc != 4) {
        perror("wrong number of arguments received");
        exit(EXIT_ERROR);
    }

    drone_id = atoi(argv[1]);
    pipe_write = atoi(argv[2]);
    script_file_path = argv[3];
    

    // parse script file
    FILE *fp = fopen(script_file_path, "r");
    if(fp == NULL){
        perror("fopen failed");
        exit(EXIT_ERROR);
    }

    char line[MAX_LINE_LENGTH];
    Position drone_positions[MAX_POSITIONS];
    
    // skip header
    if(fgets(line, MAX_LINE_LENGTH, fp) == NULL){
        perror("no header found");
        exit(EXIT_ERROR);
    }
    // read file
    int i = 0;
    while(fgets(line, MAX_LINE_LENGTH, fp) != NULL){
        sscanf(line, "%d,%d,%d,%d", &drone_positions[i].time_step, &drone_positions[i].x, &drone_positions[i].y, &drone_positions[i].z);
        i++;
    }

    fclose(fp);
    
    // sigaction
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    // simulation
    char buffer[BUFFER_SIZE];
    for(int i = 0; i < SIMULATION_STEPS; i++){
        printf("[Drone %d] Sending position: (%d, %d, %d) at time step %d\n", drone_id, drone_positions[i].x, drone_positions[i].y, drone_positions[i].z, drone_positions[i].time_step);
        int len = snprintf(buffer, BUFFER_SIZE, "%d:%d:%d,%d,%d",drone_id, i,drone_positions[i].x,drone_positions[i].y,drone_positions[i].z);
        if(write(pipe_write, buffer, len) == -1){
            perror("write failed");
            exit(EXIT_ERROR);
        }
        
        usleep(50000);
    }

    close(pipe_write);
    

    return 0;

}


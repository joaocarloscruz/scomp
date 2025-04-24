#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_CHILDREN 5
#define NUMBER_POSITIONS 100000
#define CHUNK_SIZE (NUMBER_POSITIONS/NUM_CHILDREN)


void handle_signal(int signo){
    printf("The target has been found. All children will stop execution now!\n");
    exit(0);
}

int main(){
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    int i;
    pid_t children_pid[NUM_CHILDREN];
    int arr[NUMBER_POSITIONS];
    int target = 312;
	
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handle_signal;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);
    populate_array(arr);

        

    for(i = 0; i < NUM_CHILDREN;i++){
	children_pid[i] = fork();
	if(children_pid[i] < 0){
	    perror("fork failed");
	    exit(-1);
	}

	//child process
	if( children_pid[i] == 0){
	    int result = search_target(target, i * CHUNK_SIZE, arr);
	    if( result != -1){
		printf("[Child %d - PID %d] Number %d found\n", i + 1,getpid(), target);
		exit(i + 1); //exit with the child number
            }
		exit(0); // didn't find target
	}
        // parent

    }


    // parent process
    int status;
    pid_t terminated_pid;
    pid_t child_pid;
    int found = 0;
    while((terminated_pid = waitpid(-1, &status, 0)) > 0){
	if(WIFEXITED(status)){
	    int exit_status =  WEXITSTATUS(status);
	    if(exit_status > 0  && exit_status < NUM_CHILDREN){
		found++;
		printf("The target %d was found on the child [%d]\n", target, terminated_pid);     
                for(int i = 0; i < NUM_CHILDREN; i++){
                    if(children_pid[i] !=  terminated_pid){
                        kill(children_pid[i], SIGUSR1);
                    }
                }
            }
	} else {
	    perror("There was an issue with the execution");
	}

    }

    if(found == 0){
	printf("The target wasn't found\n");

    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
double elapsed = (end_time.tv_sec - start_time.tv_sec) +
                 (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
printf("Elapsed time: %.6f seconds\n", elapsed);



	return 0;
}


int search_target(int target, int start_pos, int arr[]){
	int end_pos = start_pos + CHUNK_SIZE;
	for(int i = start_pos; i < end_pos; i++){
		if( arr[i] == target){
			return i;
		}
	}
	return -1;
	
}

void populate_array(int arr[]){
	for(int i = 0; i < NUMBER_POSITIONS; i++){
		arr[i] = i;
	}
}



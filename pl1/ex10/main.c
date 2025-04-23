#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILDREN 5
#define NUMBER_POSITIONS 100000
#define CHUNK_SIZE (NUMBER_POSITIONS/NUM_CHILDREN)


int main(){
	int i;
	pid_t pid;
	int arr[NUMBER_POSITIONS];
	int target = 312;
	
	populate_array(arr);

	for(i = 0; i < NUM_CHILDREN;i++){
		pid = fork();
		if(pid < 0){
			perror("fork failed");
			exit(-1);
		}
		//child process
		if( pid == 0){
			int result = search_target(target, i * CHUNK_SIZE, arr);
			if( result != -1){
				printf("[Child %d - PID %d] Number %d found\n", i + 1,getpid(), target);
				exit(i + 1);
			}
			exit(-1);
		}

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
				child_pid = terminated_pid;
				found++;
			} 
		} else {
			perror("There was an issue with the execution");
		}

	}

	if(found > 0) {
		printf("The target %d was found on the child [%d]\n", target, child_pid);

	} else {
		printf("No target was found\n");

	}



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


#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
	
	if(argc < 2){
		perror("No arguments provided");
		exit(-1);
	}

	pid_t pid;
	for(int i = 1; i < argc; i++){
		pid = fork();

		if(pid == 0){
			char* arr[] = {argv[i], NULL};
			execvp(argv[i], arr);
			perror("execvp failed");
			exit(1);
		}
	}
	// parent code 
	int count = 0;
	pid_t term_pid;
	int status;
	
	while((term_pid = waitpid(-1, &status, 0)) > 0){
		printf("Waiting... %d\n", count++);
	}


	return 0;
}

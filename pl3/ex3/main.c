#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <>

#define P_READ 0
#define P_WRITE 1

int main(){
	int fd[2];
	pid_t pid;
	char buffer[SIZE];
	ssize_t bytesRead;
	int filefd;

	if(pipe(fd) == -1){
		perror("pipe failed");
		exit(EXIT_FAILURE);

	}

	pid = fork();

	if(pid < 0){
		perror("fork failed");
		exit(EXIT_FAILURE);
	}


	//child code
	if(pid == 0){
		close(fd[P_WRITE]);
		
		

		exit(1);
	}

	//parent code
	
	close(fd[P_READ]);

	if(write


			
	return 0;

}


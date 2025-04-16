#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define P_READ 0
#define P_WRITE 1

int main(){
    int fd[2];
	pid_t pid;

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
               
    }

    return 0;

}

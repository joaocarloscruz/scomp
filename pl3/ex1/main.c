#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


#define P_READ 0
#define P_WRITE 1


int main(){
	int fd[2];
	pid_t pid;
	pid_t child_pid;

	if(pipe(fd) == -1){	
		perror("pipe failed");
		exit(-1);
	}

	pid = fork();
	
	if(pid < 0) {
		perror("fork error");
		exit(-1);
	}

	// parent code
	if(pid > 0){
		close(fd[P_READ]);
		printf("Parent: My PID is [%d]\n",pid);
		if(write(fd[P_WRITE], &pid, sizeof(pid)) == -1){
			perror("write error");
			exit(-1);
		
		} 
		close(fd[P_WRITE]);
		wait(NULL);
		
	}

	// child code
	if(pid == 0){
		close(fd[P_WRITE]);
		if(read(fd[P_READ], &child_pid, sizeof(child_pid)) == -1){
			perror("read error");
			exit(-1);
		}
		close(fd[P_READ]);
		printf("Child: My parent's PID is [%d]\n", child_pid);
	}



	return 0;
}

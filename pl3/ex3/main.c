#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define P_READ 0
#define P_WRITE 1
#define SIZE 512

int main(){
	int fd[2];
	pid_t pid;
	char buffer[SIZE];
	ssize_t bytesRead;

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
		if(read(fd[P_READ], buffer, SIZE) < 0){
			perror("error receiving data");
			exit(EXIT_FAILURE);
		}
		printf("%s\n", buffer);
		close(fd[P_READ]);
		exit(1);
	}

	//parent code
	close(fd[P_READ]);
	FILE *fprt;
	fprt = fopen("text.txt", "r");
	if(fprt == NULL){
		perror("fopen failed");
		kill(pid, SIGKILL);
		exit(EXIT_FAILURE);
	}


	fgets(buffer, SIZE, fprt);
	bytesRead = strlen(buffer);

	if(write(fd[P_WRITE], buffer, bytesRead) < 0){
		perror("error sending data");
		kill(pid, SIGKILL);
		exit(EXIT_FAILURE);
	}
	close(fd[P_WRITE]);
	wait(NULL);
	fclose(fprt);

			
	return 0;

}


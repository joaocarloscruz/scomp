#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define P_READ 0
#define P_WRITE 1

int main(){
	int fd[2];
	pid_t pid;
	int number;
	char name[50];

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
		if(read(fd[P_READ], &number, sizeof(int)) == -1){
			perror("read error");
			exit(EXIT_FAILURE);
		}
		if(read(fd[P_READ], name, 50) == -1){
			perror("read error");
			exit(EXIT_FAILURE);
		}
		close(fd[P_READ]);

		printf("number %d", number);
		printf("name %s", name);
		
	
	}


	//parent code
	if(pid > 0){
		close(fd[P_READ]);
		printf("Enter an integer number:\n");
		scanf("%d", &number);
		int c;
		do{
			c = getchar();
		} while( c!= EOF && c != '\n');
		
		printf("Enter a name:\n");
	
		if(fgets(name, 50, stdin) == -1){
			perror("Error reading name\n");
			exit(EXIT_FAILURE);
			
		}

		size_t len = strlen(name);
		if(len > 0 && name[len - 1] == 'n'){
			name[len - 1] = '\0';
		}

		if(write(fd[P_WRITE], &number, sizeof(int) == -1)){
			perror("write error");
			exit(EXIT_FAILURE);		
		}
		

		if(write(fd[P_WRITE], name, len == -1)){
			perror("write error");
			exit(EXIT_FAILURE);		
		}
		close(fd[P_WRITE]);

		wait(NULL);
	}

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>


#define P_READ 0
#define P_WRITE 1

int main(){
	int pipe_to_children[2], pipe_from_factorial[2];
	pid_t pid;

	if(pipe(pipe_to_children) == -1 || pipe(pipe_from_factorial) == -1){
		perror("fork error");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if(pid==0){
		//child process
		close(pipe_to_children[P_WRITE]);
		close(pipe_from_factorial[P_READ]);
		dup2(pipe_to_children[P_READ], STDIN_FILENO);
		dup2(pipe_from_factorial[P_WRITE], STDOUT_FILENO);
		execlp("./factorial", "factorial", NULL);
		perror("exec error");
		exit(EXIT_FAILURE);
	}

	// parent
	close(pipe_to_children[P_READ]);
	close(pipe_from_factorial[P_WRITE]);

	int num;
	printf("Insert the number to do the factorial\n");
	fflush(stdout);
	scanf("%d",&num);

	char num_str[20]; 
   	int len = snprintf(num_str, sizeof(num_str), "%d", num);

    	if (len < 0 || (size_t)len >= sizeof(num_str)) {
        	perror("snprintf error or buffer too small");
       		exit(EXIT_FAILURE);
    	}

    	if(write(pipe_to_children[P_WRITE], num_str, len) == -1) {
        	perror("write error");
        	exit(EXIT_FAILURE);
    	}	
	
	close(pipe_to_children[P_WRITE]);
	char buffer[100];

	ssize_t bytes_read; 

	if((bytes_read = read(pipe_from_factorial[P_READ], buffer, sizeof(buffer) - 1)) == -1){
		perror("read failed");
		exit(EXIT_FAILURE);
	}

	buffer[bytes_read] = '\0'; 
        printf("Factorial result from child: %s", buffer);

	close(pipe_from_factorial[P_READ]);

	waitpid(pid, NULL, 0);


	return 0;
}

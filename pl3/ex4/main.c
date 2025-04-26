#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

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
		char* line = NULL;
		size_t len = 0;
		ssize_t bytes_written = 0;
		close(fd[P_READ]);
		while(1){
			printf("Write a string\n");
			bytes_written = getline(&line, &len, stdin);
			if(bytes_written == -1){
				break;
			}
			if(strncmp(line, "exit", 4) == 0){
				kill(getppid(), SIGKILL);
				raise(SIGKILL);
			}
			bytes_written = write(fd[P_WRITE], line, len);

            if (bytes_written < 0) {
                perror("write error");
                break;
            }
			memset(line, 0, 512);
		}
    }

	// parent code
	if(pid > 0){
		close(fd[P_WRITE]);
		char line[512];
		ssize_t bytes_read = 0;
		while(1){
			if(read(fd[P_READ], line, 512) < 0){
				perror("rad error");
				break;
			}
			if(bytes_read == -1){
				break;
			}
			printf("Received: ");
			printf("%s", line);

			// check if upper or lower for each character
			int len = strlen(line);
			for(int i = 0; i < len; i++){
				if(line[i] >= 'a' && line[i] <= 'z'){
					line[i] = line[i] - 32;
				}
				else if(line[i] >= 'A' && line[i] <= 'Z'){
					line[i] = line[i] + 32;
				}
			}

			printf("Transformed: ");
			printf("%s", line);
	
			memset(line, 0, 512);
		}

	}

    return 0;

}

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 10

int main(){
	pid_t pid;
	char* line = NULL;
	size_t len = 0;
	ssize_t read; // signed integer, value may return error, as in getline()
	char* token;

	do{
		printf("joaocruz@DESKTOP-1096XQ:~");
		
		read = getline(&line, &len, stdin);
	
		
		// remove trailing \n
		if (read > 0 && line[read - 1] == '\n') {
			line[read - 1] = '\0';
		}
		
		const char* delimiters = " \t";
		char* args[MAX_ARGS];
		token = strtok(line, delimiters);

		if(token == NULL){
			args[0] = NULL;
			continue;

		}

		args[0] = token;

		int end;
		for(int i = 1; i < MAX_ARGS; i++){
			token = strtok(NULL, delimiters);
			if (token == NULL) {
				break;
			}
			end++;
			args[i] = token;
			
		} 
		if(end < MAX_ARGS){
			args[end] = NULL;
		}
		
		if(args[0] != NULL){
			pid = fork();
			if ( pid < 0 ){
				perror("fork failed");
				exit(-1);
			}

			if (pid == 0){
			// child process
				execvp(args[0], args);
				perror("exec failed");
				exit(-1);
			}

		// parent

			waitpid(-1,NULL, 0);
		}


	
	}while(strcmp(line, "end") != 0);

	if(line != NULL){
		free(line);
	}
	
        return 0;

}

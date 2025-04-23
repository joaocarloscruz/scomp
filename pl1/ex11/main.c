#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


char create_twins(pid_t list[2]){
	for(int i = 0; i < 2; i++){
		list[i] = fork();
		if( i == 0 && list[i] == 0){
			return 'a';
		}
		if(i == 1 && list[i] == 0){
			return 'b';
		}
	}
	return 'p';
}

int main(){
	pid_t child_pid[6];
	char res;

	for(int i = 0; i < 3; i ++){
		res = create_twins(&child_pid[i * 2]);
		if( res == 'a' || res == 'b' ){
			// child 
			printf("[Child %d][Parent %d] i = %d\n", getpid(), getppid(),i);
			exit((int)res);
	 	} else  { // res = 'p'
			printf("[Parent %d] i = %d\n", getpid(), i);
		}
	}

	// parent
	pid_t wait_pid;
	int status;
	for(int i = 0; i < 6 ; i++){
		wait_pid = waitpid(child_pid[i], &status, 0);
		if(wait_pid > 0){
			if(WIFEXITED(status)){
				char exit_status = (char) WEXITSTATUS(status);
				printf("[Parent: Child %d] [Value %c]\n", wait_pid, exit_status);
			}
		}

	}
	return 0;
}

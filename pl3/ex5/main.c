#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define P_READ 0
#define P_WRITE 1
#define SIZE 1000


int main(){
	int fd[2];
    pid_t pid;
    int arr1[SIZE];
    int arr2[SIZE];

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

    //parent code


    return 0;

  }

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

// The number of processes created is 7
// "I am the father" will print 7 times.

// to get only 3 children, we can have to the else, after the sleep(1), return 0;


int main(void) { 
    pid_t pid;
    int f, status;

    for (f = 0; f < 3 ; f++) { 
        pid = fork();

	if( pid < 0){
		perror("Erro ao criar o processo \n");
		exit(-1);
	}
        if (pid > 0) {
                printf("I am the father \n");

        } else {
            sleep(1);
	
	    exit(f+1);
        }


    }

    for(f = 0; f < 3; f++){
	    wait(&status);
	    printf("Filho terminou e retornou %d\n", WEXITSTATUS(status));
    }

    return 0;
}

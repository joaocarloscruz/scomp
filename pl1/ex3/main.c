#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

//a). The number of processes created is 16, with fork() being executed 15 times;
//b) Concurrent Programming is displayed 16 times, 1 being from the parent and the other 15 from each of the children


int main(void) {
    pid_t pid;
    int i;

    for (i = 0; i < 4; i++){
        pid = fork();
	}
    printf(" Concurrent Programming \n ");
    return 0;
}

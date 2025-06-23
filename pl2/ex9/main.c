#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

void handler(int signum){
    printf("Handled the signal\n");


}


int main(){
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);       
    sigaddset(&sa.sa_mask, SIGUSR2);

    if(sigaction(SIGUSR1, &sa, NULL) == -1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    pid_t pid; 
    pid = fork();

    if(pid == 0){
        // child
        printf("Child: Started\n");
        while (1) {
            pause(); 
        }

    } else {
        // parent
        while (1) {
            printf("Parent: Sleeping for 2 seconds...\n");
            sleep(2); 
            printf("Parent: Sending SIGUSR1 to child [%d]\n", pid);

            if (kill(pid, SIGUSR1) == -1) {
                perror("failed to send SIGUSR1");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
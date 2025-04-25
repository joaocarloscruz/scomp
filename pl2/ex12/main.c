#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define NUM_PROCESSES 1

void signal_handler(int signo, siginfo_t *info, void *context){
    usleep(100000);
    char msg [80];
    int length;
    length = snprintf(msg, 80, "I captured a %s sent by the process with PID %d \n", strsignal(signo) ,info->si_pid );
    write (STDOUT_FILENO , msg , length +1);

}



int main() {
    pid_t pid;

    struct sigaction act1;

    memset(&act1, 0, sizeof(struct sigaction));
    act1.sa_sigaction = signal_handler;
    act1.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGUSR1, &act1, NULL);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {

            kill(getppid(), SIGUSR1);
            printf("Sending SIGUSR1 to Parent [%d]\n", getppid());
            exit(EXIT_SUCCESS);
        }
    }
    
    pause();
    sleep(2);
   return 0;
}

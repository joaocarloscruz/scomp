#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define NUM_PROCESSES 50

volatile int success_count = 0;
volatile int processes_finished = 0;

void handle_success(int signo) {
    success_count++;
    processes_finished++;
    
}

void handle_failure(int signo) {
    processes_finished++;
   
}

int simulate1() {
    srand(time(NULL) ^ getpid());
    printf("[Child %d] sim1 sleeping...\n", getpid());
    usleep((rand() % 100000) + 100000);
    if (((rand() % 100) + 1) == 1) {
        return 1;
    }
    return 0;
}

int simulate2() {
    printf("[Child %d] sim2 sleeping...\n", getpid());
    usleep((rand() % 100000) + 100000);
    if (((rand() % 100) + 1) == 1) {
        return 1;
    }
    return 0;
}

int main() {
    pid_t children_pid[NUM_PROCESSES];
    pid_t pid;

    struct sigaction act1, act2;

    memset(&act1, 0, sizeof(struct sigaction));
    memset(&act2, 0, sizeof(struct sigaction));
    act1.sa_handler = handle_success;
    act2.sa_handler = handle_failure;
    act1.sa_flags = SA_RESTART;
    act2.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &act1, NULL);
    sigaction(SIGUSR2, &act2, NULL);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            int res = simulate1();
            if (res == 1) {
                kill(getppid(), SIGUSR1);
            } else {
                kill(getppid(), SIGUSR2);
            }
            raise(SIGSTOP);
            //pause();
            res = simulate2();
            exit(EXIT_SUCCESS);
        } else {
            children_pid[i] = pid;
        }
    }

    while(processes_finished < 25){
        pause();
    }
   
    int flag = 0;
    if (success_count == 0) {
        printf("Inefficient algorithm\n");
        flag = 1;
    } else {
        printf("Efficient algorithm! Heading for second simulation\n");
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (flag == 1) {
            kill(children_pid[i], SIGKILL);
        } else {
            kill(children_pid[i], SIGCONT);
        }
    }

    sleep(5);
    printf("Finished the algorithm computation");
    for(int i = 0; i < NUM_PROCESSES; i++){
        waitpid(children_pid[i], NULL, 0);
    }
    return 0;
}

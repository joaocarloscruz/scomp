#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define MAX 50

volatile int finished_count = 0;
volatile int success_count = 0;
pid_t child_pids[MAX];

void parent_sig_handler(int signum) {
    if (signum == SIGUSR1) {
        success_count++;
        finished_count++;
        printf("Parent: Success %d, Finished %d\n", success_count, finished_count);
    } else if (signum == SIGUSR2) {
        finished_count++;
        printf("Parent: Failure %d, Finished %d\n", success_count, finished_count);
    }
    if (finished_count == 25) {
        if (success_count == 0) {
            printf("\nParent: Inefficient Algorithm!\n");
            for (int j = 0; j < MAX; j++) {
                kill(child_pids[j], SIGTERM);
            }
        } else {
            printf("\nParent: At least one success. Signaling children to start simulate2.\n");
            for (int j = 0; j < MAX; j++) {
                kill(child_pids[j], SIGUSR1);
            }
        }
    }
}

void child_sig_handler(int signum) {
    if (signum == SIGUSR1) {
        printf("Child %d: Start simulate2.\n", getpid());
    }
}

int simulate1() {
    srand(time(NULL) ^ (getpid() << 16));
    sleep(rand() % 3);
    int result = rand() % 2;
    printf("Child %d: simulate1() %d\n", getpid(), result);
    return result;
}

int simulate2() {
    srand(time(NULL) ^ (getpid() << 16));
    sleep(rand() % 2);
    printf("Child %d: simulate2() finished.\n", getpid());
    return 0;
}

int main() {
    pid_t pid;
    struct sigaction act;

    act.sa_handler = parent_sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

    act.sa_handler = child_sig_handler; // Change the handler for the child
    sigemptyset(&act.sa_mask);         // Make sure to reset the mask and flags if needed
    act.sa_flags = 0;

    int i = 0;

    do {
        pid = fork();
        if (pid < 0) {
            perror("Error in fork");
            exit(EXIT_FAILURE);
        }
        child_pids[i] = pid;
        i++;
    } while (i < MAX && pid > 0);

    if (pid == 0) {
        sigaction(SIGUSR1, &act, NULL); // Use the same 'act' struct in the child
        int result_simulate1 = simulate1();
        if (result_simulate1 == 1) {
            kill(getppid(), SIGUSR1);
        } else {
            kill(getppid(), SIGUSR2);
        }
        printf("Child %d: simulate1() done, waiting...\n", getpid());
        pause();

        simulate2();
        printf("Child %d: simulate2() done.\n", getpid());
        exit(EXIT_SUCCESS);
    }

    if (pid > 0) {
        for (int j = 0; j < MAX; j++) {
            if (wait(NULL) == -1 && errno != ECHILD) {
                perror("wait");
            }
        }
        printf("Parent finished.\n");
    }

    return 0;
}
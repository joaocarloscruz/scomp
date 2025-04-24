#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

typedef struct {
    char cmd[32];
    int time;
} command_t;

volatile sig_atomic_t flag = 0;

void alarm_handler(int sig) {
    if (sig == SIGALRM) {
        flag = 1;
    }
}

int main() {
    command_t commands[] = {
        {"ls", 3}, {"echo Hello!", 5}, {"sleep 10", 4}, {"date", 2}
    };
    int num_commands = sizeof(commands) / sizeof(commands[0]);

    pid_t children_pid[num_commands];

    for (int i = 0; i < num_commands; i++) {
        children_pid[i] = fork();

        if (children_pid[i] < 0) {
            perror("fork failed");
            for (int k = 0; k < i; k++) {
                kill(children_pid[k], SIGKILL);
                waitpid(children_pid[k], NULL, 0);
            }
            exit(EXIT_FAILURE);
        }

        if (children_pid[i] == 0) {
            char cmd_copy[sizeof(commands[i].cmd)];
            strcpy(cmd_copy, commands[i].cmd);

            char *argv[10];
            int argc = 0;
            char *token = strtok(cmd_copy, " ");

            while (token != NULL && argc < 9) {
                argv[argc++] = token;
                token = strtok(NULL, " ");
            }
            argv[argc] = NULL;

            execvp(argv[0], argv);

            perror("execvp failed");
            exit(EXIT_FAILURE);
        }

        if (children_pid[i] > 0) {
            kill(children_pid[i], SIGSTOP);
        }
    }

    if (signal(SIGALRM, alarm_handler) == SIG_ERR) {
        perror("signal handler setup failed");
         for (int k = 0; k < num_commands; k++) {
            kill(children_pid[k], SIGKILL);
            waitpid(children_pid[k], NULL, 0);
        }
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < num_commands; j++) {
        pid_t current_child_pid = children_pid[j];
        int command_timeout = commands[j].time;
        int status;
        pid_t finished_pid;

        flag = 0;

        printf(" Running command %d ('%s') with timeout %d s...\n", j + 1, commands[j].cmd, command_timeout);

        alarm(command_timeout);

        if (kill(current_child_pid, SIGCONT) == -1) {
            perror("Failed to SIGCONT child");
            // This child might be problematic, skip its wait
            alarm(0); // Cancel alarm for this command
            continue;
        }

        do {
            finished_pid = waitpid(current_child_pid, &status, 0);
        } while (finished_pid == -1 && errno == EINTR);

        alarm(0);

        if (finished_pid == -1) {
             perror("waitpid error after SIGCONT");
             for (int k = j + 1; k < num_commands; k++) {
                 kill(children_pid[k], SIGKILL);
                 waitpid(children_pid[k], NULL, 0);
             }
             exit(EXIT_FAILURE);
        }


        if (flag == 1) {
            printf("Command %d ('%s') timed out.\n", j + 1, commands[j].cmd);
            if (kill(current_child_pid, SIGKILL) == -1 && errno != ESRCH) {
                 perror("Failed to SIGKILL timed-out child");
            }
            waitpid(current_child_pid, &status, 0);
        } else {
            printf("Command %d ('%s') finished.\n", j + 1, commands[j].cmd);
        }
    }

    for(int k = 0; k < num_commands; k++) {
        waitpid(children_pid[k], NULL, WNOHANG);
    }


    printf("All commands processed.\n");

    return 0;
}

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

pid_t child_pid;

void sigusr1_handler(int signum){

}

void sigusr2_handler(int signum){

}

void sigint_handler(int signum){
    kill(child_pid, SIGKILL);
    raise(SIGKILL);
}

int main(){
    pid_t pid;
    FILE *fptr;
    
    // the next part of sigprocmask is optional 
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    struct sigaction act1,act2, sigint;
    memset(&act1,0, sizeof(struct sigaction));
    memset(&act2,0, sizeof(struct sigaction));
    memset(&sigint,0, sizeof(struct sigaction));
    act1.sa_handler = sigusr1_handler;
    act2.sa_handler = sigusr2_handler;
    sigint.sa_handler = sigint_handler;
    sigint.sa_flags = SA_RESTART;
    act1.sa_flags = SA_RESTART;
    act2.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &act1, NULL);
    sigaction(SIGUSR2, &act2, NULL);
    sigaction(SIGINT, &sigint, NULL);


    //create child
    pid = fork();
    if(pid < 0){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    
    //child process
    /*if(pid == 0){
        int i = 0;
        sigset_t child_mask;
        sigfillset(&child_mask);
        sigdelset(&child_mask, SIGINT);
        sigdelset(&child_mask, SIGUSR2);
       
        pid_t parent_pid = getppid();
        while(1){
            fptr = fopen("data.txt", "w");
            if(fptr == NULL){
                perror("file error");
                exit(EXIT_FAILURE);
            }
            fprintf(fptr, "%d\n", i);
            i++;
            fclose(fptr);
            sleep(1); // optional
            kill(parent_pid, SIGUSR1);
            
            sigsuspend(&child_mask);
            //pause(); //waiting for parent to read

        }
        

    }
    //parent process
    if(pid > 0){
        child_pid = pid;
        sigset_t parent_mask;
        sigfillset(&parent_mask);
        sigdelset(&parent_mask, SIGINT);
        sigdelset(&parent_mask, SIGUSR1);
        while(1){
    
            sigsuspend(&parent_mask);
            //pause(); // waiting for child to write
            char line[100];
            fptr = fopen("data.txt", "r");
            if(fptr == NULL){
                perror("file error");
                exit(EXIT_FAILURE);
            }
            fgets(line, 100, fptr);
            printf("%s", line);
            fclose(fptr);
            sleep(1); //optional
            kill(pid, SIGUSR2);
        }
    }*/

    if(pid == 0){
        int i = 0;
        sigset_t child_mask;
        /*sigfillset(&child_mask);
        sigdelset(&child_mask, SIGINT);
        sigdelset(&child_mask, SIGUSR2);
        sigprocmask(SIG_SETMASK, &child_mask, NULL);*/
        sigemptyset(&child_mask);
        sigaddset(&child_mask, SIGUSR2);
        sigprocmask(SIG_UNBLOCK, &child_mask, &mask);
       
        pid_t parent_pid = getppid();
        while(1){
            fptr = fopen("data.txt", "w");
            if(fptr == NULL){
                perror("file error");
                exit(EXIT_FAILURE);
            }
            fprintf(fptr, "%d\n", i);
            i++;
            fclose(fptr);
            sleep(1); // optional
            kill(parent_pid, SIGUSR1);
            
            pause(); //waiting for parent to read

        }
        

    }
    //parent process
    if(pid > 0){
        child_pid = pid;
        sigset_t parent_mask;
        /*sigfillset(&parent_mask);
        sigdelset(&parent_mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &parent_mask, NULL);
        */
        sigemptyset(&parent_mask);
        sigaddset(&parent_mask, SIGUSR1);
        sigprocmask(SIG_UNBLOCK, &parent_mask, &mask);
        
        while(1){
    
            
            pause(); // waiting for child to write
            char line[100];
            fptr = fopen("data.txt", "r");
            if(fptr == NULL){
                perror("file error");
                exit(EXIT_FAILURE);
            }
            fgets(line, 100, fptr);
            printf("%s", line);
            fclose(fptr);
            sleep(1); //optional
            kill(pid, SIGUSR2);
        }
    }


    return 0;
}


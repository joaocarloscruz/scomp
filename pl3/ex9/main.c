#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define P_READ 0
#define P_WRITE 1

int main(){
    int fd1[2];
    int fd2[2];
    pid_t pid;
    int balance = 20;
    int stat;
    int random_number;
    srand(time(NULL)); // Seed for the parent process

    if(pipe(fd1) == -1){
        perror("pipe error");
        exit(-1);
    }

    if(pipe(fd2) == -1){
        perror("pipe error");
        exit(-1);
    }

    pid =  fork();

    if( pid < 0){
        perror("fork error");
    }
    // parent code
    if( pid > 0){
        close(fd1[P_READ]);
        close(fd2[P_WRITE]);
        while( balance > 4){
            int child_number;
            random_number = (rand() % 5) + 1;
            // Notify child to make a bet and send the generated number
            stat = 1;
            if(write(fd1[P_WRITE], &stat, sizeof(int)) == -1){
                perror("write error (status to child)");
                exit(-1);
            }
            if(write(fd1[P_WRITE], &random_number, sizeof(int)) == -1){
                perror("write error (number to child)");
                exit(-1);
            }
            printf("Parent : Generated number: %d\n", random_number);

            // Read the child's bet
            if(read(fd2[P_READ], &child_number, sizeof(int)) == -1){
                perror("read error (child's bet)");
                exit(-1);
            }

            printf("Parent : Child's bet: %d\n", child_number);

            if(child_number == random_number){
                balance += 10;
                printf("Parent : Child guessed correctly!\n");
            } else {
                balance -= 5;
                printf("Parent : Child guessed incorrectly.\n");
            }
            printf("Parent : Balance is now %d\n", balance);

            // Send the updated balance to the child
            if(write(fd1[P_WRITE], &balance, sizeof(int)) == -1){
                perror("write error (balance to child)");
                exit(-1);
            }
        }
        // Notify child that the game ended
        stat = 0;
        if(write(fd1[P_WRITE], &stat, sizeof(int)) == -1){
            perror("write error (end status to child)");
            exit(-1);
        }
        close(fd1[P_WRITE]);
        close(fd2[P_READ]);
        wait(NULL); // Wait for the child to terminate
    }

    // child code
    if( pid == 0 ) {
        srand(getpid()); // Seed for the child process using its PID
        int status;
        int parent_number;
        int current_balance;
        close(fd1[P_WRITE]);
        close(fd2[P_READ]);

        while(1) {
            // Read status from parent
            if(read(fd1[P_READ], &status, sizeof(int)) == -1){
                perror("read error (status from parent)");
                exit(-1);
            }

            if(status == 0){
                printf("Child : Parent's balance too low, game over.\n");
                break;
            } else {
                // Read the random number from the parent
                if(read(fd1[P_READ], &parent_number, sizeof(int)) == -1){
                    perror("read error (number from parent)");
                    exit(-1);
                }
                int child_number = (rand() % 5) + 1;
                printf("Child : Generated number %d\n", child_number);

                // Send the bet to the parent
                if(write(fd2[P_WRITE], &child_number, sizeof(int)) == -1){
                    perror("write error (bet to parent)");
                    exit(-1);
                }

                // Read the updated balance from the parent
                if(read(fd1[P_READ], &current_balance, sizeof(int)) == -1){
                    perror("read error (balance from parent)");
                    exit(-1);
                }
                printf("Child : Current balance: %d\n", current_balance);
            }
        }
        close(fd1[P_READ]);
        close(fd2[P_WRITE]);
        exit(0);
    }
    return 0;
}
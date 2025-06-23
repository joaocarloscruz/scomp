#include <stdio.h>
#include <stdlib.h>

int main(){
    int balance = 25;
    int guess; 
    int bet;
    int number;
    printf("Welcome to Guess!\nYour starting balance is %d\n", balance);
    while(balance > 0){
        printf("Enter your guess from 1-5\n");
        scanf("%d", &guess);
        while(guess < 1 | guess > 5 ){
            printf("Your guess is not between 1-5. Try again\n");
            scanf("%d", &guess);
        }
        printf("Enter your bet. Your current balance is %d\n", balance);
        scanf("%d", &bet);
        while(bet > balance){
            printf("Your bet is more than your balance. Try again\n");
            scanf("%d", &bet);
        }

        balance = balance - bet;

        pid_t pid = fork();
        if(pid == 0){
            number = execl("./cheat", "cheat", NULL);
        }
        //parent waits for child
        int status;
        waitpid(pid, &status, 0);        
        if (WIFEXITED(status)) {
            number = WEXITSTATUS(status);
        }


        if(guess == number){
            balance = balance + (2 * bet);
            printf("You won! Your balance is %d\n", balance);
        } else{
            printf("You lost! Your balance is %d\nThe correct number was %d\n", balance, number);
        }

    }
    printf("You're out of money to bet. Goodbye!\n");
    return 0;
}
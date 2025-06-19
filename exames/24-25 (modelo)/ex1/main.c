#include <stdio.h>

#define NUM_TICKETS 50
#define NUM_SCANNERS 5

#define P_READ 0 
#define P_WRITE 1

typedef struct{
    int ticket_id;
    char visitor_name[80];
    int is_valid;
} Ticket;

typedef struct {
    int scanner_id;
    int ticket_id_to_check;
} ScanRequest;

Ticket tickets[NUM_TICKETS]

int main(){
    int request[2];
    if (pipe(request) == -1) {
        perror("Failed to create trequest pipe");
        exit(EXIT_FAILURE);
    }
    int response[NUM_SCANNERS][2];
    for (int i = 0; i < NUM_SCANNERS; i++) {
        if (pipe(reponse[i]) == -1) {
            perror("Failed to create response pipe");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < NUM_SCANNERS; i++){
        pid_t pid = fork();
        if(pid == 0){ // child process
            close(request[P_READ]);
            close(response[i][P_WRITE]);
            // close all other pipes
            for (int j = 0; j < NUM_SCANNERS; j++) {
                if (j != i) {
                    close(response[j][P_READ]);
                    close(response[j][P_WRITE]);
                }
            }



            
            
            close(request[P_WRITE]);
            close(response[i][P_READ]);
            exit(EXIT_SUCCESS);
        }
    }
    
    // parent process

    





    return 0;
}
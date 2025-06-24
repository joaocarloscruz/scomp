#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define P_READ 0
#define P_WRITE 1

#define NUM_SCANNERS 5

typedef struct{
    int barcode;
    char product[100];
    double price;
} product_t;

typedef struct {
    int barcode_id;    
    int child_index;   
} request_t;

product_t product_database[] = {
    {1001, "Banana (1kg)", 1.25},
    {1002, "Orange Juice (1L)", 2.99},
    {1003, "Yogurt (Pack of 4)", 2.50},
    {1004, "Ground Beef (500g)", 5.80},
    {1005, "Pasta Sauce (Jar)", 1.90},
    {1006, "Cereal (Box)", 3.75},
    {1007, "Toilet Paper (4 Rolls)", 4.00},
    {1008, "Dish Soap", 2.10},
    {1009, "Shampoo", 5.00},
    {1010, "Frozen Pizza", 6.50}
};

void generate_random_barcode(int reply_pipe_fd, int scanner_index);

int main(){
    int request_pipe_fd[2];
    if (pipe(request_pipe_fd) == -1) {
        perror("request pipe creation failed");
        exit(EXIT_FAILURE);
    }

    int reply_pipe_fds[NUM_SCANNERS][2];
    for (int i = 0; i < NUM_SCANNERS; i++) {
        if (pipe(reply_pipe_fds[i]) == -1) {
            perror("reply pipe creation failed");
            exit(EXIT_FAILURE);
        }
    }
    pid_t pid[NUM_SCANNERS];
    for (int i = 0; i < NUM_SCANNERS; ++i) {
        pid[i] = fork();
        if (pid[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid[i] == 0) {
            // child 
            srand(time(NULL) ^ getpid());
            close(request_pipe_fd[P_READ]);
            close(reply_pipe_fds[i][P_WRITE]);

            for (int j = 0; j < NUM_SCANNERS; j++) {
                if (j != i) {
                    close(reply_pipe_fds[j][P_READ]);
                    close(reply_pipe_fds[j][P_WRITE]);
                }
            }

            int count = 0;
            while (count < 10) {
                generate_random_barcode(request_pipe_fd[P_WRITE], i);
                product_t reply;
                if (read(reply_pipe_fds[i][P_READ], &reply, sizeof(product_t)) > 0) {
                    printf("Scanner %d received barcode %d\n", i, reply.barcode);
                    printf("Product: %s\n", reply.product);
                    printf("Price: %.2f\n", reply.price);
                }
                count++;
            }
            exit(EXIT_SUCCESS);
        } 
    }
    close(request_pipe_fd[P_WRITE]);
    for (int i = 0; i < NUM_SCANNERS; i++) {
        close(reply_pipe_fds[i][P_READ]);
    }
    request_t request;
    while (read(request_pipe_fd[P_READ], &request, sizeof(request_t)) > 0) {
        product_t reply;
        printf("Parent received barcode %d from Scanner %d \n", request.barcode_id, request.child_index);
        for (int j = 0; j < sizeof(product_database) / sizeof(product_database[0]); j++) {
            if (product_database[j].barcode == request.barcode_id) {
                reply.barcode = product_database[j].barcode;
                strcpy(reply.product, product_database[j].product);
                reply.price = product_database[j].price;
                if (write(reply_pipe_fds[request.child_index][P_WRITE], &reply, sizeof(product_t)) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                break;
            }
        }
    }
    for (int i = 0; i < NUM_SCANNERS; i++) {
        waitpid(pid[i], NULL, 0); 
    }
    printf("Parent: All children terminated. Exiting.\n");
    return 0;
}


void generate_random_barcode(int request_pipe_fd, int scanner_index){
    int number = (rand() % 10) + 1001;
    request_t request;
    request.barcode_id = number;
    request.child_index = scanner_index;
    if(write(request_pipe_fd, &request, sizeof(request_t)) == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }
}


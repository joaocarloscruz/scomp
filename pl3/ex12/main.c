#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define P_READ 0
#define P_WRITE 1
#define M1_OUTPUT 5
#define M2_OUTPUT 5
#define M3_OUTPUT 10
#define M4_OUTPUT 100
#define TOTAL_PRODUCTION 1000

#define M1_BATCHES_NEEDED (TOTAL_PRODUCTION / M4_OUTPUT * (M4_OUTPUT / M3_OUTPUT) * (M3_OUTPUT / M2_OUTPUT) * (M2_OUTPUT / M1_OUTPUT)) // 200
#define M3_INPUT_BATCHES_PER_OUTPUT (M3_OUTPUT / M2_OUTPUT) // 10/5 = 2
#define M4_INPUT_BATCHES_PER_OUTPUT (M4_OUTPUT / M3_OUTPUT) // 100/10 = 10


int main(){
    int fd_m1[2], fd_m2[2], fd_m3[2], fd_m4[2];
    pid_t pid_m1, pid_m2, pid_m3, pid_m4;

    if(pipe(fd_m1) == -1 || pipe(fd_m2) == -1 || pipe(fd_m3) == -1 || pipe(fd_m4) == -1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_m1 = fork();
    if(pid_m1 < 0){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(pid_m1 == 0){
        close(fd_m1[P_READ]);
        close(fd_m2[P_READ]);
        close(fd_m2[P_WRITE]);
        close(fd_m3[P_READ]);
        close(fd_m3[P_WRITE]);
        close(fd_m4[P_READ]);
        close(fd_m4[P_WRITE]);
        int i = 0;
        int write_data = M1_OUTPUT;
        while(i < M1_BATCHES_NEEDED){
            if(write(fd_m1[P_WRITE], &write_data, sizeof(int)) < 0){
                perror("M1 write error");
                exit(EXIT_FAILURE);
            }
            printf("M1 : %d pieces were cut\n", write_data);
            usleep(50000);
            i++;
        }
        close(fd_m1[P_WRITE]);
        exit(EXIT_SUCCESS);
    }

    pid_m2 = fork();
    if(pid_m2 < 0){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(pid_m2 == 0){
        close(fd_m1[P_WRITE]);
        close(fd_m2[P_READ]);
        close(fd_m3[P_READ]);
        close(fd_m3[P_WRITE]);
        close(fd_m4[P_READ]);
        close(fd_m4[P_WRITE]);
        int read_value;
        int batches_folded = 0;
        while(read(fd_m1[P_READ], &read_value, sizeof(int)) > 0){
             if (read_value == M2_OUTPUT) {
                int write_data = M2_OUTPUT;
                if(write(fd_m2[P_WRITE], &write_data, sizeof(int)) < 0){
                    perror("M2 write error");
                    exit(EXIT_FAILURE);
                }
                printf("M2 : %d pieces were folded\n", write_data);
                batches_folded++;
                usleep(50000);
             } else {
                 fprintf(stderr, "M2: Received unexpected batch size %d from M1\n", read_value);
             }
        }
        printf("M2: Finished folding %d batches.\n", batches_folded);
        close(fd_m1[P_READ]);
        close(fd_m2[P_WRITE]);
        exit(EXIT_SUCCESS);
    }

    pid_m3 = fork();
    if(pid_m3 < 0){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if(pid_m3 == 0){
        close(fd_m1[P_READ]);
        close(fd_m1[P_WRITE]);
        close(fd_m2[P_WRITE]);
        close(fd_m3[P_READ]);
        close(fd_m4[P_READ]);
        close(fd_m4[P_WRITE]);
        int read_value;
        int current_pieces = 0;
        int batches_welded = 0;
        int write_data = M3_OUTPUT;

        while(read(fd_m2[P_READ], &read_value, sizeof(int)) > 0){
             if (read_value == M2_OUTPUT) {
                current_pieces += read_value;

                if(current_pieces >= M3_OUTPUT){
                     if (current_pieces == M3_OUTPUT) {
                        if(write(fd_m3[P_WRITE], &write_data, sizeof(int)) < 0){
                            perror("M3 write error");
                            exit(EXIT_FAILURE);
                        }
                        printf("M3 : %d pieces were welded\n", write_data);
                        batches_welded++;
                        current_pieces = 0;
                        usleep(100000);
                     } else {
                        fprintf(stderr, "M3: Unexpected accumulation amount %d after reading %d\n", current_pieces, read_value);
                        current_pieces = 0;
                     }
                }
             } else {
                 fprintf(stderr, "M3: Received unexpected batch size %d from M2\n", read_value);
             }
        }
        printf("M3: Finished welding %d batches.\n", batches_welded);
        close(fd_m2[P_READ]);
        close(fd_m3[P_WRITE]);
        exit(EXIT_SUCCESS);
    }

    pid_m4 = fork();
    if(pid_m4 < 0){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(pid_m4 == 0){
        close(fd_m1[P_READ]);
        close(fd_m1[P_WRITE]);
        close(fd_m2[P_READ]);
        close(fd_m2[P_WRITE]);
        close(fd_m3[P_WRITE]);
        close(fd_m4[P_READ]);
        int read_value;
        int current_pieces = 0;
        int batches_packed = 0;
        int total_packed = 0;
        int write_data = M4_OUTPUT;

        while(total_packed < TOTAL_PRODUCTION && read(fd_m3[P_READ], &read_value, sizeof(int)) > 0){
             if (read_value == M3_OUTPUT) {
                 current_pieces += read_value;

                 if(current_pieces >= M4_OUTPUT){
                      if (current_pieces == M4_OUTPUT) {
                        if(write(fd_m4[P_WRITE], &write_data, sizeof(int)) < 0){
                            perror("M4 write error");
                            exit(EXIT_FAILURE);
                        }
                        printf("M4 : %d pieces were packed\n", write_data);
                        batches_packed++;
                        total_packed += write_data;
                        current_pieces = 0;
                        usleep(200000);
                        if (total_packed >= TOTAL_PRODUCTION) {
                            break;
                        }
                     } else {
                         fprintf(stderr, "M4: Unexpected accumulation amount %d after reading %d\n", current_pieces, read_value);
                         current_pieces = 0;
                     }
                 }
             } else {
                 fprintf(stderr, "M4: Received unexpected batch size %d from M3\n", read_value);
             }
        }
        printf("M4: Finished packing %d batches, totaling %d pieces.\n", batches_packed, total_packed);
        close(fd_m3[P_READ]);
        close(fd_m4[P_WRITE]);
        exit(EXIT_SUCCESS);
    }

    close(fd_m1[P_WRITE]);
    close(fd_m1[P_READ]);
    close(fd_m2[P_WRITE]);
    close(fd_m2[P_READ]);
    close(fd_m3[P_WRITE]);
    close(fd_m3[P_READ]);
    close(fd_m4[P_WRITE]);

    int received_packed_pieces;
    int total_inventory = 0;

    printf("A1 (Parent): Waiting for packed goods from M4.\n");
    while(total_inventory < TOTAL_PRODUCTION && read(fd_m4[P_READ], &received_packed_pieces, sizeof(int)) > 0){
        printf("A1 (Parent): Received %d packed pieces from M4.\n", received_packed_pieces);
        total_inventory += received_packed_pieces;
        printf("A1 (Parent): Current inventory: %d/%d.\n", total_inventory, TOTAL_PRODUCTION);
    }
    printf("A1 (Parent): Finished receiving from M4. Final inventory: %d.\n", total_inventory);


    waitpid(pid_m1, NULL, 0);
    waitpid(pid_m2, NULL, 0);
    waitpid(pid_m3, NULL, 0);
    waitpid(pid_m4, NULL, 0);

    close(fd_m4[P_READ]);

    printf("A1 (Parent): All machines finished.\n");

    return 0;
}